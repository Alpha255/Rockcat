#include "RHI/Vulkan/VulkanBarrier.h"
#include "RHI/Vulkan/VulkanCommandBuffer.h"
#include "RHI/Vulkan/VulkanRHI.h"
#include "RHI/Vulkan/VulkanDevice.h"
#include "RHI/Vulkan/VulkanQueue.h"

template<class PipelineStageFlags, class PipelineStageFlagBits>
PipelineStageFlags GetPipelineStageFlagsForImageLayout(vk::ImageLayout Layout)
{
	PipelineStageFlags Flags;

	switch (Layout)
	{
	case vk::ImageLayout::eTransferSrcOptimal:
	case vk::ImageLayout::eTransferDstOptimal:
		Flags = PipelineStageFlagBits::eTransfer;
		break;
	case vk::ImageLayout::eColorAttachmentOptimal:
		Flags = PipelineStageFlagBits::eColorAttachmentOutput;
		break;
	case vk::ImageLayout::eDepthStencilAttachmentOptimal:
	case vk::ImageLayout::eDepthAttachmentOptimal:
	case vk::ImageLayout::eStencilAttachmentOptimal:
		Flags = PipelineStageFlagBits::eEarlyFragmentTests | PipelineStageFlagBits::eLateFragmentTests;
		break;
	case vk::ImageLayout::eShaderReadOnlyOptimal:
		Flags = PipelineStageFlagBits::eFragmentShader;
		break;
	case vk::ImageLayout::eDepthStencilReadOnlyOptimal:
	case vk::ImageLayout::eDepthReadOnlyStencilAttachmentOptimal:
	case vk::ImageLayout::eDepthAttachmentStencilReadOnlyOptimal:
	case vk::ImageLayout::eDepthReadOnlyOptimal:
	case vk::ImageLayout::eStencilReadOnlyOptimal:
		Flags = PipelineStageFlagBits::eFragmentShader | PipelineStageFlagBits::eEarlyFragmentTests | PipelineStageFlagBits::eLateFragmentTests;
		break;
	case vk::ImageLayout::ePresentSrcKHR:
		Flags = PipelineStageFlagBits::eBottomOfPipe;
		break;
	case vk::ImageLayout::eFragmentDensityMapOptimalEXT:
		Flags = PipelineStageFlagBits::eFragmentDensityProcessEXT;
		break;
	case vk::ImageLayout::eFragmentShadingRateAttachmentOptimalKHR:
		Flags = PipelineStageFlagBits::eFragmentShadingRateAttachmentKHR;
		break;
	case vk::ImageLayout::eGeneral:
	case vk::ImageLayout::eUndefined:
		Flags = PipelineStageFlagBits::eTopOfPipe;
		break;
	case vk::ImageLayout::eAttachmentOptimal:
		Flags = PipelineStageFlagBits::eEarlyFragmentTests | PipelineStageFlagBits::eLateFragmentTests;
		break;
	case vk::ImageLayout::eReadOnlyOptimal:
		Flags = PipelineStageFlagBits::eFragmentShader | PipelineStageFlagBits::eEarlyFragmentTests | PipelineStageFlagBits::eLateFragmentTests;
		break;
	default:
		assert(0);
		break;
	}

	return Flags;
}

template<class AccessFlags, class AccessFlagBits>
AccessFlags GetAccessFlagsForImageLayout(vk::ImageLayout Layout)
{
	AccessFlags Flags;

	switch (Layout)
	{
	case vk::ImageLayout::eTransferSrcOptimal:
		Flags = AccessFlagBits::eTransferRead;
		break;
	case vk::ImageLayout::eTransferDstOptimal:
		Flags = AccessFlagBits::eTransferWrite;
		break;
	case vk::ImageLayout::eColorAttachmentOptimal:
		Flags = AccessFlagBits::eColorAttachmentWrite;
		break;
	case vk::ImageLayout::eDepthStencilAttachmentOptimal:
	case vk::ImageLayout::eDepthAttachmentOptimal:
	case vk::ImageLayout::eStencilAttachmentOptimal:
		Flags = AccessFlagBits::eDepthStencilAttachmentRead | AccessFlagBits::eDepthStencilAttachmentWrite;
		break;
	case vk::ImageLayout::eDepthReadOnlyStencilAttachmentOptimal:
	case vk::ImageLayout::eDepthAttachmentStencilReadOnlyOptimal:
		Flags = AccessFlagBits::eShaderRead | AccessFlagBits::eDepthStencilAttachmentRead | AccessFlagBits::eDepthStencilAttachmentWrite;
		break;
	case vk::ImageLayout::eShaderReadOnlyOptimal:
		Flags = AccessFlagBits::eShaderRead;
		break;
	case vk::ImageLayout::eDepthStencilReadOnlyOptimal:
	case vk::ImageLayout::eDepthReadOnlyOptimal:
	case vk::ImageLayout::eStencilReadOnlyOptimal:
		Flags = AccessFlagBits::eShaderRead | AccessFlagBits::eDepthStencilAttachmentRead;
		break;
	case vk::ImageLayout::ePresentSrcKHR:
		break;
	case vk::ImageLayout::eFragmentDensityMapOptimalEXT:
		Flags = AccessFlagBits::eFragmentDensityMapReadEXT;
		break;
	case vk::ImageLayout::eFragmentShadingRateAttachmentOptimalKHR:
		Flags = AccessFlagBits::eFragmentShadingRateAttachmentReadKHR;
		break;
	case vk::ImageLayout::eGeneral:
	case vk::ImageLayout::eUndefined:
		break;
	case vk::ImageLayout::eAttachmentOptimal:
		Flags = AccessFlagBits::eDepthStencilAttachmentRead | AccessFlagBits::eDepthStencilAttachmentWrite;
		break;
	case vk::ImageLayout::eReadOnlyOptimal:
		Flags = AccessFlagBits::eShaderRead;
		break;
	default:
		assert(0);
		break;
	}

	return Flags;
}

VulkanPipelineBarrier& VulkanPipelineBarrier::AddImageLayoutTransition(
	ERHIDeviceQueue SrcQueue, 
	ERHIDeviceQueue DstQueue, 
	vk::Image Image, 
	vk::ImageLayout SrcLayout, 
	vk::ImageLayout DstLayout, 
	const vk::ImageSubresourceRange& Subresource)
{
	uint32_t SrcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	uint32_t DstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

	if (SrcQueue != DstQueue)
	{
		SrcQueueFamilyIndex = GetDevice().GetQueue(SrcQueue).GetFamilyIndex();
		DstQueueFamilyIndex = GetDevice().GetQueue(DstQueue).GetFamilyIndex();
	}

	if (m_UseBarrier2)
	{
		auto& ImageMemoryBarrier = m_ImageMemoryBarriers2.emplace_back();
		ImageMemoryBarrier.setSrcStageMask(GetPipelineStageFlagsForImageLayout<vk::PipelineStageFlags2, vk::PipelineStageFlagBits2>(SrcLayout))
			.setSrcAccessMask(GetAccessFlagsForImageLayout<vk::AccessFlags2, vk::AccessFlagBits2>(SrcLayout))
			.setDstStageMask(GetPipelineStageFlagsForImageLayout<vk::PipelineStageFlags2, vk::PipelineStageFlagBits2>(DstLayout))
			.setDstAccessMask(GetAccessFlagsForImageLayout<vk::AccessFlags2, vk::AccessFlagBits2>(DstLayout))
			.setOldLayout(SrcLayout)
			.setNewLayout(DstLayout)
			.setSrcQueueFamilyIndex(SrcQueueFamilyIndex)
			.setDstQueueFamilyIndex(DstQueueFamilyIndex)
			.setImage(Image)
			.setSubresourceRange(Subresource);
	}
	else
	{
		auto& ImageMemoryBarrier = m_ImageMemoryBarriers.emplace_back();
		ImageMemoryBarrier.setSrcAccessMask(GetAccessFlagsForImageLayout<vk::AccessFlags, vk::AccessFlagBits>(SrcLayout))
			.setDstAccessMask(GetAccessFlagsForImageLayout<vk::AccessFlags, vk::AccessFlagBits>(DstLayout))
			.setOldLayout(SrcLayout)
			.setNewLayout(DstLayout)
			.setSrcQueueFamilyIndex(SrcQueueFamilyIndex)
			.setDstQueueFamilyIndex(DstQueueFamilyIndex)
			.setImage(Image)
			.setSubresourceRange(Subresource);

		m_SrcPipelineStageFlags |= GetPipelineStageFlagsForImageLayout<vk::PipelineStageFlags, vk::PipelineStageFlagBits>(SrcLayout);
		m_DstPipelineStageFlags |= GetPipelineStageFlagsForImageLayout<vk::PipelineStageFlags, vk::PipelineStageFlagBits>(DstLayout);
	}

	return *this;
}

void VulkanPipelineBarrier::Submit(VulkanCommandBuffer* CommandBuffer)
{
	assert(CommandBuffer);

	vk::DependencyFlags DependencyFlags;

	if (m_UseBarrier2)
	{
		if (!m_MemoryBarriers2.empty() || !m_BufferMemoryBarriers2.empty() || !m_ImageMemoryBarriers2.empty())
		{
			vk::DependencyInfo DependencyInfo;
			DependencyInfo.setDependencyFlags(DependencyFlags)
				.setMemoryBarriers(m_MemoryBarriers2)
				.setBufferMemoryBarriers(m_BufferMemoryBarriers2)
				.setImageMemoryBarriers(m_ImageMemoryBarriers2);
			CommandBuffer->GetNative().pipelineBarrier2(DependencyInfo);

			m_MemoryBarriers2.clear();
			m_BufferMemoryBarriers2.clear();
			m_ImageMemoryBarriers2.clear();
		}
	}
	else
	{
		if (!m_MemoryBarriers.empty() || !m_BufferMemoryBarriers.empty() || !m_ImageMemoryBarriers.empty())
		{
			CommandBuffer->GetNative().pipelineBarrier(
				m_SrcPipelineStageFlags, 
				m_DstPipelineStageFlags, 
				DependencyFlags, 
				m_MemoryBarriers, 
				m_BufferMemoryBarriers, 
				m_ImageMemoryBarriers);

			m_MemoryBarriers.clear();
			m_BufferMemoryBarriers.clear();
			m_ImageMemoryBarriers.clear();

			m_SrcPipelineStageFlags = vk::PipelineStageFlags(0u);
			m_DstPipelineStageFlags = vk::PipelineStageFlags(0u);
		}
	}
}
