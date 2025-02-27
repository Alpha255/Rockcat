#pragma once

#include "RHI/Vulkan/VulkanTypes.h"

class VulkanPipelineBarrier : public VkBaseDeviceResource
{
public:
	using VkBaseDeviceResource::VkBaseDeviceResource;

	VulkanPipelineBarrier& AddImageLayoutTransition(
		ERHIDeviceQueue SrcQueue, 
		ERHIDeviceQueue DstQueue, 
		vk::Image Image, 
		vk::ImageLayout SrcLayout, 
		vk::ImageLayout DstLayout, 
		const vk::ImageSubresourceRange& Subresource);

	void Submit(class VulkanCommandBuffer* CommandBuffer);
private:
	std::vector<vk::MemoryBarrier> m_MemoryBarriers;
	std::vector<vk::ImageMemoryBarrier> m_ImageMemoryBarriers;
	std::vector<vk::BufferMemoryBarrier> m_BufferMemoryBarriers;

	std::vector<vk::MemoryBarrier2> m_MemoryBarriers2;
	std::vector<vk::ImageMemoryBarrier2> m_ImageMemoryBarriers2;
	std::vector<vk::BufferMemoryBarrier2> m_BufferMemoryBarriers2;

	vk::PipelineStageFlags m_SrcPipelineStageFlags;
	vk::PipelineStageFlags m_DstPipelineStageFlags;

	bool m_UseBarrier2 = false;
};
