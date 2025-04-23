#include "RHI/Vulkan/VulkanBuffer.h"
#include "RHI/Vulkan/VulkanDevice.h"
#include "RHI/Vulkan/VulkanRHI.h"
#include "RHI/Vulkan/VulkanMemoryAllocator.h"
#include "RHI/RHIUploadManager.h"

VulkanBuffer::VulkanBuffer(const VulkanDevice& Device, const RHIBufferCreateInfo& RHICreateInfo)
	: VkHwResource(Device)
	, RHIBuffer(RHICreateInfo)
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

	size_t AlignedSize = RHICreateInfo.Size;
	vk::BufferUsageFlags UsageFlags = vk::BufferUsageFlagBits::eTransferSrc | vk::BufferUsageFlagBits::eTransferDst;

	if (EnumHasAnyFlags(RHICreateInfo.BufferUsageFlags, ERHIBufferUsageFlags::UniformBuffer))
	{
		UsageFlags |= vk::BufferUsageFlagBits::eUniformBuffer;
		AlignedSize = Align(RHICreateInfo.Size, GetDevice().GetPhysicalDeviceLimits().minUniformBufferOffsetAlignment);
	}
	if (EnumHasAnyFlags(RHICreateInfo.BufferUsageFlags, ERHIBufferUsageFlags::IndexBuffer))
	{
		UsageFlags |= vk::BufferUsageFlagBits::eIndexBuffer;
	}
	if (EnumHasAnyFlags(RHICreateInfo.BufferUsageFlags, ERHIBufferUsageFlags::VertexBuffer))
	{
		UsageFlags |= vk::BufferUsageFlagBits::eVertexBuffer;
	}
	if (EnumHasAnyFlags(RHICreateInfo.BufferUsageFlags, ERHIBufferUsageFlags::IndirectBuffer))
	{
		UsageFlags |= vk::BufferUsageFlagBits::eIndirectBuffer;
	}
	if (EnumHasAnyFlags(RHICreateInfo.BufferUsageFlags, ERHIBufferUsageFlags::StructuredBuffer))
	{
		UsageFlags |= vk::BufferUsageFlagBits::eStorageBuffer;
		AlignedSize = Align(RHICreateInfo.Size, GetDevice().GetPhysicalDeviceLimits().minStorageBufferOffsetAlignment);
	}
	if (EnumHasAnyFlags(RHICreateInfo.BufferUsageFlags, ERHIBufferUsageFlags::UnorderedAccess))
	{
		UsageFlags |= vk::BufferUsageFlagBits::eStorageTexelBuffer;
		AlignedSize = Align(RHICreateInfo.Size, GetDevice().GetPhysicalDeviceLimits().minTexelBufferOffsetAlignment);
	}
	if (EnumHasAnyFlags(RHICreateInfo.BufferUsageFlags, ERHIBufferUsageFlags::ShaderResource))
	{
		UsageFlags |= vk::BufferUsageFlagBits::eUniformTexelBuffer;
	}
	if (EnumHasAnyFlags(RHICreateInfo.BufferUsageFlags, ERHIBufferUsageFlags::AccelerationStructure))
	{
		UsageFlags |= vk::BufferUsageFlagBits::eAccelerationStructureStorageKHR;
		/// #TODO Align
	}

	vk::BufferCreateInfo CreateInfo;
	CreateInfo.setSize(AlignedSize)
		.setUsage(UsageFlags)
		.setSharingMode(vk::SharingMode::eExclusive);

	VERIFY_VK(GetNativeDevice().createBuffer(&CreateInfo, VK_ALLOCATION_CALLBACKS, &m_Native));

	AllocateAndBindMemory(RHICreateInfo.AccessFlags);

	if (RHICreateInfo.InitialData)
	{
		RHIUploadManager::Get().QueueUploadBuffer(this, RHICreateInfo.InitialData, RHICreateInfo.Size, 0u);
		//if (m_Memory.HostVisible && !m_Memory.HostCoherent)
		//{
		//	FlushMappedRange(VK_WHOLE_SIZE, 0u);
		//}
	}

	VkHwResource::SetObjectName(RHICreateInfo.Name.c_str());
}

void VulkanBuffer::AllocateAndBindMemory(ERHIDeviceAccessFlags AccessFlags)
{
	m_Memory = VulkanMemoryAllocator::Get().Allocate(GetNative(), AccessFlags);
	assert(m_Memory.Native);

	/// #TODO VK_KHR_bind_memory2: On some implementations, it may be more efficient to batch memory bindings into a single command.
	GetNativeDevice().bindBufferMemory(m_Native, m_Memory.Native, 0u);
}

void* VulkanBuffer::Map(ERHIMapMode Mode, size_t Size, size_t Offset)
{
	assert(m_Memory.HostVisible && (Size == VK_WHOLE_SIZE || Offset + Size <= m_Size));

	if (m_MappedMemory.Memory && (m_MappedMemory.Mode != Mode || m_MappedMemory.Size != Size || m_MappedMemory.Offset != Offset))
	{
		Unmap();
	}

	if (!m_MappedMemory.Memory)
	{
		VERIFY_VK(GetNativeDevice().mapMemory(m_Memory.Native, Offset, Size, vk::MemoryMapFlags(0u), &m_MappedMemory.Memory));
	}

	return m_MappedMemory.Memory;
}

void VulkanBuffer::Unmap()
{
	assert(m_Memory.HostVisible);

	if (m_MappedMemory.Memory)
	{
		GetNativeDevice().unmapMemory(m_Memory.Native);
		m_MappedMemory.Memory = nullptr;
	}
}

void VulkanBuffer::FlushMappedRange(size_t Size, size_t Offset)
{
	/// Within each range described by pMemoryRanges, each set of nonCoherentAtomSize bytes in that range is flushed if any byte in that set has been written by the host since it was first host mapped, 
	/// or the last time it was flushed. If pMemoryRanges includes sets of nonCoherentAtomSize bytes where no bytes have been written by the host, those bytes must not be flushed.
	/// Unmapping non-coherent memory does not implicitly flush the host mapped memory, and host writes that have not been flushed may not ever be visible to the device.
	/// However, implementations must ensure that writes that have not been flushed do not become visible to any other memory.

	/// #TODO Align to nonCoherentAtomSize
	
	assert(!m_Memory.HostCoherent && m_MappedMemory.Memory);
	assert(Offset + Size <= m_Size || (Size == VK_WHOLE_SIZE && Offset < m_Size));

	vk::MappedMemoryRange MappedRange;
	MappedRange.setMemory(m_Memory.Native)
		.setSize(Size)
		.setOffset(Offset);

	VERIFY_VK(GetNativeDevice().flushMappedMemoryRanges(1u, &MappedRange));
}

void VulkanBuffer::InvalidateMappedRange(size_t Size, size_t Offset)
{
	/// #TODO Align to nonCoherentAtomSize

	assert(!m_Memory.HostCoherent && m_MappedMemory.Memory);
	assert(Offset + Size <= m_Size || (Size == VK_WHOLE_SIZE && Offset < m_Size));

	vk::MappedMemoryRange MappedRange;
	MappedRange.setMemory(m_Memory.Native)
		.setSize(Size)
		.setOffset(Offset);

	VERIFY_VK(GetNativeDevice().invalidateMappedMemoryRanges(1u, &MappedRange));
}

VulkanBuffer::~VulkanBuffer()
{
	if (m_MappedMemory.Memory)
	{
		Unmap();
	}

	if (m_Memory.Native)
	{
		GetNativeDevice().freeMemory(m_Memory.Native);
		m_Memory.Native = nullptr;
	}
}
