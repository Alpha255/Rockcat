#include "Colorful/Vulkan/VulkanMemoryAllocator.h"
#include "Colorful/Vulkan/VulkanDevice.h"

NAMESPACE_START(RHI)

VulkanMemoryAllocator::VulkanMemoryAllocator(VulkanDevice* Device)
	: m_Device(Device)
{
	assert(Device);
	vkGetPhysicalDeviceMemoryProperties(m_Device->PhysicalDevice(), &m_MemoryProperties);
}

uint32_t VulkanMemoryAllocator::GetMemoryTypeIndex(uint32_t MemTypeBits, VkMemoryPropertyFlags MemPropertyFlags) const
{
	for (uint32_t MemTypeIndex = 0u; MemTypeIndex < m_MemoryProperties.memoryTypeCount; ++MemTypeIndex)
	{
		if (((1 << MemTypeIndex) & MemTypeBits) &&
			((m_MemoryProperties.memoryTypes[MemTypeIndex].propertyFlags & MemPropertyFlags) == MemPropertyFlags))
		{
			return MemTypeIndex;
		}
	}

	return InvalidMemTypeIndex;
}

VulkanDeviceMemory VulkanMemoryAllocator::Alloc(VkBuffer Buffer, EDeviceAccessFlags AccessFlags)
{
	assert(Buffer != VK_NULL_HANDLE);

	VkMemoryRequirements Requirements;
	vkGetBufferMemoryRequirements(m_Device->Get(), Buffer, &Requirements);

	return Alloc(Requirements, AccessFlags);
}

VulkanDeviceMemory VulkanMemoryAllocator::Alloc(VkImage Image, EDeviceAccessFlags AccessFlags)
{
	assert(Image != VK_NULL_HANDLE);

	VkMemoryRequirements Requirements;
	vkGetImageMemoryRequirements(m_Device->Get(), Image, &Requirements);

	return Alloc(Requirements, AccessFlags);
}

VulkanDeviceMemory VulkanMemoryAllocator::Alloc(const VkMemoryRequirements& Requirements, EDeviceAccessFlags AccessFlags)
{
	/// VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT bit specifies that memory allocated with this type can be mapped for host access using vkMapMemory.

	/// VK_MEMORY_PROPERTY_HOST_COHERENT_BIT bit specifies that the host cache management commands vkFlushMappedMemoryRanges and vkInvalidateMappedMemoryRanges 
	/// are not needed to flush host writes to the device or make device writes visible to the host, respectively.

	/// VK_MEMORY_PROPERTY_HOST_CACHED_BIT bit specifies that memory allocated with this type is cached on the host. 
	/// Host memory accesses to uncached memory are slower than to cached memory, however uncached memory is always host coherent.

	VkMemoryPropertyFlags MemPropertyFlags = 0u;
	if (AccessFlags == EDeviceAccessFlags::GpuRead || AccessFlags == EDeviceAccessFlags::GpuReadWrite)
	{
		MemPropertyFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
	}
	else if (AccessFlags == EDeviceAccessFlags::GpuReadCpuWrite)
	{
		MemPropertyFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
	}
	else
	{
		assert(0);
	}

	if (EnumHasAnyFlags(AccessFlags, EDeviceAccessFlags::CpuRead))
	{
		MemPropertyFlags |= VK_MEMORY_PROPERTY_HOST_CACHED_BIT;
	}

	auto MemTypeIndex = GetMemoryTypeIndex(Requirements.memoryTypeBits, MemPropertyFlags);
	if (MemTypeIndex == InvalidMemTypeIndex)
	{
		MemPropertyFlags &= ~VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
		MemTypeIndex = GetMemoryTypeIndex(Requirements.memoryTypeBits, MemPropertyFlags);
		assert(MemTypeIndex != InvalidMemTypeIndex);
	}

	VkMemoryAllocateInfo AllocateInfo
	{
		VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
		nullptr,
		Requirements.size,
		MemTypeIndex
	};

	VkDeviceMemory DeviceMemory = VK_NULL_HANDLE;
	VERIFY_VK(vkAllocateMemory(m_Device->Get(), &AllocateInfo, VK_ALLOCATION_CALLBACKS, &DeviceMemory));
	return VulkanDeviceMemory
	{
		DeviceMemory,
		Requirements.size,
		EnumHasAnyFlags(MemPropertyFlags, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT),
		EnumHasAnyFlags(MemPropertyFlags, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT),
		EnumHasAnyFlags(MemPropertyFlags, VK_MEMORY_PROPERTY_HOST_CACHED_BIT)
	};
}

NAMESPACE_END(RHI)