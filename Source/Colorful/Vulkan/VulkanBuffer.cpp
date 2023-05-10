#include "Colorful/Vulkan/VulkanBuffer.h"
#include "Colorful/Vulkan/VulkanDevice.h"
#include "Colorful/Vulkan/VulkanMemoryAllocator.h"
#include "Colorful/Vulkan/VulkanQueue.h"

NAMESPACE_START(RHI)

VulkanBuffer::VulkanBuffer(VulkanDevice* Device, const BufferDesc& Desc)
	: VkHWObject(Device)
{
	/// If a memory object does not have the VK_MEMORY_PROPERTY_HOST_COHERENT_BIT property, 
	/// then vkFlushMappedMemoryRanges must be called in order to guarantee that writes to the memory object from the host are made available to the host domain, 
	/// where they can be further made available to the device domain via a domain operation. Similarly, 
	/// vkInvalidateMappedMemoryRanges must be called to guarantee that writes which are available to the host domain are made visible to host operations.
	
	/// If the memory object does have the VK_MEMORY_PROPERTY_HOST_COHERENT_BIT property flag, 
	/// writes to the memory object from the host are automatically made available to the host domain.Similarly, 
	/// writes made available to the host domain are automatically made visible to the host.

	/// The memory type that allows us to access it from the CPU may not be the most optimal memory type for the graphics card itself to read from. 
	/// The most optimal memory has the VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT flag and is usually not accessible by the CPU on dedicated graphics cards. 
	
	/// One staging buffer in CPU accessible memory to upload the data from the vertex array to, and the final vertex buffer in device local memory.

	/// VK_SHARING_MODE_EXCLUSIVE specifies that access to any range or image subresource of the object will be exclusive to a single queue family at a time
	/// VK_SHARING_MODE_CONCURRENT specifies that concurrent access to any range or image subresource of the object from multiple queue families is supported

	/// Always include TRANSFER_SRC since hardware vendors confirmed it wouldn't have any performance cost and we need it for some debug functionalities.[From UE]

	/// https://www.khronos.org/registry/vulkan/specs/1.3-extensions/man/html/VkBufferUsageFlagBits.html

	size_t AlignedSize = Desc.Size;
	VkBufferUsageFlags UsageFlags = VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;

	if (EnumHasAnyFlags(Desc.UsageFlags, EBufferUsageFlags::UniformBuffer))
	{
		UsageFlags |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
		AlignedSize = Align(Desc.Size, m_Device->PhysicalLimits().minUniformBufferOffsetAlignment);
		RequiredState = EResourceState::UniformBuffer;
	}
	if (EnumHasAnyFlags(Desc.UsageFlags, EBufferUsageFlags::IndexBuffer))
	{
		UsageFlags |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
		RequiredState = EResourceState::IndexBuffer;
	}
	if (EnumHasAnyFlags(Desc.UsageFlags, EBufferUsageFlags::VertexBuffer))
	{
		UsageFlags |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
		RequiredState = EResourceState::VertexBuffer;
	}
	if (EnumHasAnyFlags(Desc.UsageFlags, EBufferUsageFlags::IndirectBuffer))
	{
		UsageFlags |= VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;
		RequiredState = EResourceState::IndirectArgument;
	}
	if (EnumHasAnyFlags(Desc.UsageFlags, EBufferUsageFlags::StructuredBuffer))
	{
		UsageFlags |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
		AlignedSize = Align(Desc.Size, m_Device->PhysicalLimits().minStorageBufferOffsetAlignment);
	}
	if (EnumHasAnyFlags(Desc.UsageFlags, EBufferUsageFlags::UnorderedAccess))
	{
		UsageFlags |= VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT;
		AlignedSize = Align(Desc.Size, m_Device->PhysicalLimits().minTexelBufferOffsetAlignment);
		RequiredState = EResourceState::UnorderedAccess;
	}
	if (EnumHasAnyFlags(Desc.UsageFlags, EBufferUsageFlags::ShaderResource))
	{
		UsageFlags |= VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT;
		RequiredState = EResourceState::ShaderResource;
	}
	if (EnumHasAnyFlags(Desc.UsageFlags, EBufferUsageFlags::AccelerationStructure))
	{
		UsageFlags |= VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR;
		RequiredState = EResourceState::AccelerationStructure;
		/// #TODO Align
	}

	VkBufferCreateInfo CreateInfo
	{
		VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
		nullptr,
		0u,
		AlignedSize,
		UsageFlags,
		VK_SHARING_MODE_EXCLUSIVE,
		0u,
		nullptr
	};
	VERIFY_VK(vkCreateBuffer(m_Device->Get(), &CreateInfo, VK_ALLOCATION_CALLBACKS, Reference()));

	m_DeviceMemory = VulkanMemoryAllocator::Get().Alloc(Get(), Desc.AccessFlags);

	VERIFY_VK(vkBindBufferMemory(m_Device->Get(), Get(), m_DeviceMemory.Handle, 0u));

	const bool8_t IsVolatile = Desc.AccessFlags == EDeviceAccessFlags::GpuReadCpuWrite;

	if (IsVolatile)
	{
		Map(VK_WHOLE_SIZE, 0ull);
	}

	if (Desc.InitialData)
	{
		if (IsVolatile)
		{
			VERIFY(memcpy_s(m_MappedMemory, Desc.Size, Desc.InitialData, Desc.Size) == 0);
		}
#if false
		else
		{
			ScopedBufferMapper Mapper(this, AlignedSize, 0u);
			VERIFY(memcpy_s(m_MappedMemory, Desc.Size, Desc.InitialData, Desc.Size) == 0);
		}
#endif

		if (m_DeviceMemory.IsHostVisible)
		{
			if (!m_DeviceMemory.IsCoherent)
			{
				FlushMappedRange(VK_WHOLE_SIZE, 0u);
			}
		}
		else
		{
			auto Command = m_Device->GetOrAllocateCommandBuffer(EQueueType::Transfer, ECommandBufferLevel::Primary, true, true);

			Command->CopyBuffer(this, Desc.InitialData, Desc.Size, 0u, 0u);

			if (m_Device->Options().BatchResourceSubmit)
			{
				m_Device->Queue(EQueueType::Transfer)->QueueSubmit(std::vector<ICommandBufferSharedPtr>{Command});
			}
			else
			{
				m_Device->Queue(EQueueType::Transfer)->Submit(std::vector<ICommandBufferSharedPtr>{Command});
			}
		}
	}
}

void* VulkanBuffer::Map(size_t Size, size_t Offset)
{
	assert(m_DeviceMemory.IsHostVisible && m_MappedMemory == nullptr && (Size == VK_WHOLE_SIZE || Offset + Size <= m_DeviceMemory.Size));

	VERIFY_VK(vkMapMemory(m_Device->Get(), m_DeviceMemory.Handle, Offset, Size, 0u, &m_MappedMemory));
	
	return m_MappedMemory;
}

void VulkanBuffer::Unmap()
{
	assert(m_DeviceMemory.IsHostVisible && m_MappedMemory);

	vkUnmapMemory(m_Device->Get(), m_DeviceMemory.Handle);
	m_MappedMemory = nullptr;
}

void VulkanBuffer::FlushMappedRange(size_t Size, size_t Offset)
{
	/// Within each range described by pMemoryRanges, each set of nonCoherentAtomSize bytes in that range is flushed if any byte in that set has been written by the host since it was first host mapped, 
	/// or the last time it was flushed. If pMemoryRanges includes sets of nonCoherentAtomSize bytes where no bytes have been written by the host, those bytes must not be flushed.
	/// Unmapping non-coherent memory does not implicitly flush the host mapped memory, and host writes that have not been flushed may not ever be visible to the device.
	/// However, implementations must ensure that writes that have not been flushed do not become visible to any other memory.

	/// #TODO Align to nonCoherentAtomSize
	
	assert(!m_DeviceMemory.IsCoherent);
	
	assert(m_MappedMemory);

	assert(Offset + Size <= m_DeviceMemory.Size || (Size == VK_WHOLE_SIZE && Offset < m_DeviceMemory.Size));

	VkMappedMemoryRange MemRange
	{
		VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,
		nullptr,
		m_DeviceMemory.Handle,
		Offset,
		Size
	};
	VERIFY_VK(vkFlushMappedMemoryRanges(m_Device->Get(), 1u, &MemRange));
}

void VulkanBuffer::InvalidateMappedRange(size_t Size, size_t Offset)
{
	/// #TODO Align to nonCoherentAtomSize

	assert(!m_DeviceMemory.IsCoherent);

	assert(m_MappedMemory);

	assert(Offset + Size <= m_DeviceMemory.Size || (Size == VK_WHOLE_SIZE && Offset < m_DeviceMemory.Size));

	VkMappedMemoryRange MemRange
	{
		VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,
		nullptr,
		m_DeviceMemory.Handle,
		Offset,
		Size
	};
	VERIFY_VK(vkInvalidateMappedMemoryRanges(m_Device->Get(), 1u, &MemRange));
}

bool8_t VulkanBuffer::Update(const void* Data, size_t Size, size_t SrcOffset, size_t DstOffset)
{
	assert(Data && Size && Size <= m_DeviceMemory.Size);

	if (m_MappedMemory)
	{
		VERIFY(memcpy_s(
			reinterpret_cast<byte8_t*>(m_MappedMemory) + DstOffset, 
			Size, 
			reinterpret_cast<const byte8_t*>(Data) + SrcOffset, 
			Size) == 0);

		if (!m_DeviceMemory.IsCoherent)
		{
			FlushMappedRange(Size, DstOffset);
		}

		return true;
	}

	return false;
}

VulkanBuffer::~VulkanBuffer()
{
	if (m_MappedMemory)
	{
		Unmap();
	}

	if (m_DeviceMemory.Handle)
	{
		vkFreeMemory(m_Device->Get(), m_DeviceMemory.Handle, VK_ALLOCATION_CALLBACKS);
		m_DeviceMemory.Handle = VK_NULL_HANDLE;
	}
	vkDestroyBuffer(m_Device->Get(), Get(), VK_ALLOCATION_CALLBACKS);
}

NAMESPACE_END(RHI)
