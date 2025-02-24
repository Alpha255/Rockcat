#include "RHI/Vulkan/VulkanMemoryAllocator.h"
#include "RHI/Vulkan/VulkanDevice.h"

VulkanMemoryAllocator::VulkanMemoryAllocator(const VulkanDevice& Device)
	: VkBaseDeviceResource(Device)
{
	GetNativePhysicalDevice().getMemoryProperties(&m_MemoryProperties);
}

uint32_t VulkanMemoryAllocator::GetMemoryTypeIndex(uint32_t MemTypeBits, vk::MemoryPropertyFlags MemPropertyFlags) const
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

VulkanDeviceMemory VulkanMemoryAllocator::Allocate(vk::Buffer Buffer, ERHIDeviceAccessFlags AccessFlags)
{
	assert(Buffer);

	vk::MemoryRequirements Requirements;
	GetNativeDevice().getBufferMemoryRequirements(Buffer, &Requirements);

	return Allocate(Requirements, AccessFlags);
}

VulkanDeviceMemory VulkanMemoryAllocator::Allocate(vk::Image Image, ERHIDeviceAccessFlags AccessFlags)
{
	assert(Image);

	vk::MemoryRequirements Requirements;
	GetNativeDevice().getImageMemoryRequirements(Image, &Requirements);

	return Allocate(Requirements, AccessFlags);
}

VulkanDeviceMemory VulkanMemoryAllocator::Allocate(const vk::MemoryRequirements& Requirements, ERHIDeviceAccessFlags AccessFlags)
{
	/// VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT bit specifies that memory allocated with this type can be mapped for host access using vkMapMemory.

	/// VK_MEMORY_PROPERTY_HOST_COHERENT_BIT bit specifies that the host cache management commands vkFlushMappedMemoryRanges and vkInvalidateMappedMemoryRanges 
	/// are not needed to flush host writes to the device or make device writes visible to the host, respectively.

	/// VK_MEMORY_PROPERTY_HOST_CACHED_BIT bit specifies that memory allocated with this type is cached on the host. 
	/// Host memory accesses to uncached memory are slower than to cached memory, however uncached memory is always host coherent.

	VulkanDeviceMemory Memory;
	vk::MemoryPropertyFlags MemPropertyFlags;
	if (AccessFlags == ERHIDeviceAccessFlags::GpuRead || AccessFlags == ERHIDeviceAccessFlags::GpuReadWrite)
	{
		MemPropertyFlags = vk::MemoryPropertyFlagBits::eDeviceLocal;
	}
	else if (AccessFlags == ERHIDeviceAccessFlags::GpuReadCpuWrite)
	{
		MemPropertyFlags = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;
		Memory.HostVisible = Memory.HostCoherent = true;
	}
	else
	{
		assert(0);
	}

	if (EnumHasAnyFlags(AccessFlags, ERHIDeviceAccessFlags::CpuRead))
	{
		MemPropertyFlags |= vk::MemoryPropertyFlagBits::eHostCached;
		Memory.HostCached = true;
	}

	auto MemTypeIndex = GetMemoryTypeIndex(Requirements.memoryTypeBits, MemPropertyFlags);
	if (MemTypeIndex == InvalidMemTypeIndex)
	{
		MemPropertyFlags &= ~vk::MemoryPropertyFlagBits::eHostCoherent;
		MemTypeIndex = GetMemoryTypeIndex(Requirements.memoryTypeBits, MemPropertyFlags);
		assert(MemTypeIndex != InvalidMemTypeIndex);
	}

	vk::MemoryAllocateInfo AllocateInfo;
	AllocateInfo.setAllocationSize(Requirements.size)
		.setMemoryTypeIndex(MemTypeIndex);

	VERIFY_VK(GetNativeDevice().allocateMemory(&AllocateInfo, VK_ALLOCATION_CALLBACKS, &Memory.Native));
	return Memory;
}