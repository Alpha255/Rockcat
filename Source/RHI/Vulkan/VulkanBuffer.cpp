#include "RHI/Vulkan/VulkanBuffer.h"
#include "RHI/Vulkan/VulkanDevice.h"
#include "Engine/Services/SpdLogService.h"
#include "Engine/RHI/RHIInterface.h"

VulkanBuffer::VulkanBuffer(const VulkanDevice& Device, const RHIBufferCreateInfo& CreateInfo)
	: VkHwResource(Device)
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

	size_t AlignedSize = CreateInfo.Size;
	vk::BufferUsageFlags UsageFlags = vk::BufferUsageFlagBits::eTransferSrc | vk::BufferUsageFlagBits::eTransferDst;

	if (EnumHasAnyFlags(CreateInfo.BufferUsageFlags, ERHIBufferUsageFlags::UniformBuffer))
	{
		UsageFlags |= vk::BufferUsageFlagBits::eUniformBuffer;
		AlignedSize = Align(CreateInfo.Size, GetDevice().GetPhysicalDeviceLimits().minUniformBufferOffsetAlignment);
		//RequiredState = EResourceState::UniformBuffer;
	}
	if (EnumHasAnyFlags(CreateInfo.BufferUsageFlags, ERHIBufferUsageFlags::IndexBuffer))
	{
		UsageFlags |= vk::BufferUsageFlagBits::eIndexBuffer;
		//RequiredState = EResourceState::IndexBuffer;
	}
	if (EnumHasAnyFlags(CreateInfo.BufferUsageFlags, ERHIBufferUsageFlags::VertexBuffer))
	{
		UsageFlags |= vk::BufferUsageFlagBits::eVertexBuffer;
		//RequiredState = EResourceState::VertexBuffer;
	}
	if (EnumHasAnyFlags(CreateInfo.BufferUsageFlags, ERHIBufferUsageFlags::IndirectBuffer))
	{
		UsageFlags |= vk::BufferUsageFlagBits::eIndirectBuffer;
		//RequiredState = EResourceState::IndirectArgument;
	}
	if (EnumHasAnyFlags(CreateInfo.BufferUsageFlags, ERHIBufferUsageFlags::StructuredBuffer))
	{
		UsageFlags |= vk::BufferUsageFlagBits::eStorageBuffer;
		AlignedSize = Align(CreateInfo.Size, GetDevice().GetPhysicalDeviceLimits().minStorageBufferOffsetAlignment);
	}
	if (EnumHasAnyFlags(CreateInfo.BufferUsageFlags, ERHIBufferUsageFlags::UnorderedAccess))
	{
		UsageFlags |= vk::BufferUsageFlagBits::eStorageTexelBuffer;
		AlignedSize = Align(CreateInfo.Size, GetDevice().GetPhysicalDeviceLimits().minTexelBufferOffsetAlignment);
		//RequiredState = EResourceState::UnorderedAccess;
	}
	if (EnumHasAnyFlags(CreateInfo.BufferUsageFlags, ERHIBufferUsageFlags::ShaderResource))
	{
		UsageFlags |= vk::BufferUsageFlagBits::eUniformTexelBuffer;
		//RequiredState = EResourceState::ShaderResource;
	}
	if (EnumHasAnyFlags(CreateInfo.BufferUsageFlags, ERHIBufferUsageFlags::AccelerationStructure))
	{
		UsageFlags |= vk::BufferUsageFlagBits::eAccelerationStructureStorageKHR;
		//RequiredState = EResourceState::AccelerationStructure;
		/// #TODO Align
	}

	auto vkCreateInfo = vk::BufferCreateInfo()
		.setSize(AlignedSize)
		.setUsage(UsageFlags)
		.setSharingMode(vk::SharingMode::eExclusive);
	VERIFY_VK(GetNativeDevice().createBuffer(&vkCreateInfo, VK_ALLOCATION_CALLBACKS, &m_Native));

	//m_DeviceMemory = VulkanMemoryAllocator::Get().Alloc(Get(), CreateInfo.AccessFlags);

	GetNativeDevice().bindBufferMemory(m_Native, m_Memory, 0u);

	if (CreateInfo.InitialData)
	{
#if false
		else
		{
			ScopedBufferMapper Mapper(this, AlignedSize, 0u);
			VERIFY(memcpy_s(m_MappedMemory, CreateInfo.Size, CreateInfo.InitialData, CreateInfo.Size) == 0);
		}
#endif

		if (m_HostVisible)
		{
			if (!m_Coherent)
			{
				FlushMappedRange(VK_WHOLE_SIZE, 0u);
			}
		}
		else
		{
			if (RHIInterface::GetGraphicsSettings().BatchResourceDataTransfer)
			{
			}
			else
			{
			}
			//auto Command = m_Device->GetOrAllocateCommandBuffer(EQueueType::Transfer, ECommandBufferLevel::Primary, true, true);

			//Command->CopyBuffer(this, CreateInfo.InitialData, CreateInfo.Size, 0u, 0u);

			//if (m_Device->Options().BatchResourceSubmit)
			//{
			//	m_Device->Queue(EQueueType::Transfer)->QueueSubmit(std::vector<ICommandBufferSharedPtr>{Command});
			//}
			//else
			//{
			//	m_Device->Queue(EQueueType::Transfer)->Submit(std::vector<ICommandBufferSharedPtr>{Command});
			//}
		}
	}
}

void* VulkanBuffer::Map(size_t Size, size_t Offset)
{
	assert(m_HostVisible && m_MappedMemory == nullptr && (Size == VK_WHOLE_SIZE || Offset + Size <= m_Size));

	VERIFY_VK(GetNativeDevice().mapMemory(m_Memory, Offset, Size, vk::MemoryMapFlags(0u), &m_MappedMemory));
	return m_MappedMemory;
}

void VulkanBuffer::Unmap()
{
	assert(m_HostVisible && m_MappedMemory);

	GetNativeDevice().unmapMemory(m_Memory);
	m_MappedMemory = nullptr;
}

void VulkanBuffer::FlushMappedRange(size_t Size, size_t Offset)
{
	/// Within each range described by pMemoryRanges, each set of nonCoherentAtomSize bytes in that range is flushed if any byte in that set has been written by the host since it was first host mapped, 
	/// or the last time it was flushed. If pMemoryRanges includes sets of nonCoherentAtomSize bytes where no bytes have been written by the host, those bytes must not be flushed.
	/// Unmapping non-coherent memory does not implicitly flush the host mapped memory, and host writes that have not been flushed may not ever be visible to the device.
	/// However, implementations must ensure that writes that have not been flushed do not become visible to any other memory.

	/// #TODO Align to nonCoherentAtomSize
	
	assert(!m_Coherent && m_MappedMemory);
	assert(Offset + Size <= m_Size || (Size == VK_WHOLE_SIZE && Offset < m_Size));

	auto MappedRange = vk::MappedMemoryRange()
		.setMemory(m_Memory)
		.setSize(Size)
		.setOffset(Offset);
	VERIFY_VK(GetNativeDevice().flushMappedMemoryRanges(1u, &MappedRange));
}

void VulkanBuffer::InvalidateMappedRange(size_t Size, size_t Offset)
{
	/// #TODO Align to nonCoherentAtomSize

	assert(!m_Coherent && m_MappedMemory);
	assert(Offset + Size <= m_Size || (Size == VK_WHOLE_SIZE && Offset < m_Size));

	auto MappedRange = vk::MappedMemoryRange()
		.setMemory(m_Memory)
		.setSize(Size)
		.setOffset(Offset);
	VERIFY_VK(GetNativeDevice().invalidateMappedMemoryRanges(1u, &MappedRange));
}

bool VulkanBuffer::Update(const void* Data, size_t Size, size_t SrcOffset, size_t DstOffset)
{
	assert(Data && Size && Size <= m_Size);

	if (m_MappedMemory)
	{
		VERIFY(memcpy_s(
			reinterpret_cast<uint8_t*>(m_MappedMemory) + DstOffset, 
			Size, 
			reinterpret_cast<const uint8_t*>(Data) + SrcOffset, 
			Size) == 0);

		if (!m_Coherent)
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

	if (m_Memory)
	{
		GetNativeDevice().freeMemory(m_Memory);
		m_Memory = nullptr;
	}
}
