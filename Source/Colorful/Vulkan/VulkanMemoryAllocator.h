#pragma once

#include "Colorful/Vulkan/VulkanBuffer.h"

NAMESPACE_START(RHI)

class VulkanMemoryAllocator : public LazySingleton<VulkanMemoryAllocator>
{
public:
	VulkanDeviceMemory Alloc(VkBuffer Buffer, EDeviceAccessFlags AccessFlags);

	VulkanDeviceMemory Alloc(VkImage Image, EDeviceAccessFlags AccessFlags);

	void Free(VkDeviceMemory) 
	{
	};
protected:
	friend class VulkanRenderer;

	ALLOW_ACCESS_LAZY(VulkanMemoryAllocator)

	VulkanMemoryAllocator(class VulkanDevice* Device);

	uint32_t GetMemoryTypeIndex(uint32_t MemTypeBits, VkMemoryPropertyFlags MemPropertyFlags) const;

	VulkanDeviceMemory Alloc(const VkMemoryRequirements &Requirements, EDeviceAccessFlags AccessFlags);
private:
	VkPhysicalDeviceMemoryProperties m_MemoryProperties{};
	class VulkanDevice* m_Device = nullptr;

	static constexpr uint32_t InvalidMemTypeIndex = ~0u;
};

NAMESPACE_END(RHI)
