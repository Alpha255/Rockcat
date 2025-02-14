#pragma once

#include "Core/Singleton.h"
#include "RHI/Vulkan/VulkanBuffer.h"

class VulkanMemoryAllocator : public LazySingleton<VulkanMemoryAllocator>, public VkBaseDeviceResource
{
public:
	vk::DeviceMemory Allocate(vk::Buffer Buffer, ERHIDeviceAccessFlags AccessFlags);
	vk::DeviceMemory Allocate(vk::Image Image, ERHIDeviceAccessFlags AccessFlags);
protected:
	ALLOW_ACCESS_LAZY(VulkanMemoryAllocator)

	VulkanMemoryAllocator(const class VulkanDevice& Device);

	uint32_t GetMemoryTypeIndex(uint32_t MemTypeBits, vk::MemoryPropertyFlags MemPropertyFlags) const;

	vk::DeviceMemory Allocate(const vk::MemoryRequirements &Requirements, ERHIDeviceAccessFlags AccessFlags);
private:
	vk::PhysicalDeviceMemoryProperties m_MemoryProperties;

	static constexpr uint32_t InvalidMemTypeIndex = ~0u;
};
