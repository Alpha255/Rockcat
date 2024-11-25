#include "RHI/Vulkan/VulkanRenderPass.h"
#include "RHI/Vulkan/VulkanDevice.h"
#include "Engine/Services/SpdLogService.h"

VulkanRenderPass::VulkanRenderPass(const VulkanDevice& Device, const RHIFrameBufferCreateInfo& CreateInfo)
	: VkHwResource(Device)
{
	/*********************************************************************************************************************
		VK_ATTACHMENT_LOAD_OP_LOAD specifies that the previous contents of the image within the render area will be preserved.
		For attachments with a depth/stencil format, this uses the access type VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT.
		For attachments with a color format, this uses the access type VK_ACCESS_COLOR_ATTACHMENT_READ_BIT.
		
		VK_ATTACHMENT_LOAD_OP_CLEAR specifies that the contents within the render area will be cleared to a uniform value,
		which is specified when a render pass instance is begun.
		For attachments with a depth/stencil format, this uses the access type VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT.
		For attachments with a color format, this uses the access type VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT.
		
		VK_ATTACHMENT_LOAD_OP_DONT_CARE specifies that the previous contents within the area need not be preserved;
		the contents of the attachment will be undefined inside the render area.
		For attachments with a depth/stencil format, this uses the access type VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT.
		For attachments with a color format, this uses the access type VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT.
	**********************************************************************************************************************/
		
	/*********************************************************************************************************************
		VK_ATTACHMENT_STORE_OP_STORE specifies the contents generated during the render pass and within the render area are written to memory.
		For attachments with a depth/stencil format, this uses the access type VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT.
		For attachments with a color format, this uses the access type VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT.
		
		VK_ATTACHMENT_STORE_OP_DONT_CARE specifies the contents within the render area are not needed after rendering, and may be discarded;
		the contents of the attachment will be undefined inside the render area.
		For attachments with a depth/stencil format, this uses the access type VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT.
		For attachments with a color format, this uses the access type VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT.
	*********************************************************************************************************************/

	assert(CreateInfo.ColorAttachments.size() > 0u);

	std::vector<vk::AttachmentReference> ColorAttachmentReferences(CreateInfo.ColorAttachments.size());
	std::vector<vk::AttachmentDescription> AttachmentDescriptions(CreateInfo.ColorAttachments.size());
	vk::AttachmentReference DepthAttachmentReference;

	for (uint32_t ColorAttachmentIndex = 0u; ColorAttachmentIndex < CreateInfo.ColorAttachments.size(); ++ColorAttachmentIndex)
	{
		AttachmentDescriptions[ColorAttachmentIndex]
			.setFormat(GetFormat(CreateInfo.ColorAttachments[ColorAttachmentIndex]->GetFormat()))
			.setSamples(vk::SampleCountFlagBits::e1)
			.setLoadOp(vk::AttachmentLoadOp::eLoad)
			.setStoreOp(vk::AttachmentStoreOp::eStore)
			.setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
			.setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
			.setInitialLayout(vk::ImageLayout::eColorAttachmentOptimal)
			.setFinalLayout(vk::ImageLayout::eColorAttachmentOptimal);

		ColorAttachmentReferences[ColorAttachmentIndex]
			.setAttachment(ColorAttachmentIndex)
			.setLayout(vk::ImageLayout::eAttachmentOptimal);
	}

	if (CreateInfo.DepthStencilAttachment)
	{
		AttachmentDescriptions.emplace_back(
			vk::AttachmentDescription()
			.setFormat(GetFormat(CreateInfo.DepthStencilAttachment->GetFormat()))
			.setSamples(vk::SampleCountFlagBits::e1)
			.setLoadOp(vk::AttachmentLoadOp::eLoad)
			.setStoreOp(vk::AttachmentStoreOp::eStore)
			.setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
			.setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
			.setInitialLayout(vk::ImageLayout::eStencilAttachmentOptimal)
			.setFinalLayout(vk::ImageLayout::eStencilAttachmentOptimal));

		DepthAttachmentReference
			.setAttachment(static_cast<uint32_t>(AttachmentDescriptions.size()) - 1u)
			.setLayout(vk::ImageLayout::eStencilAttachmentOptimal);
	}

	auto SubpassDescription = vk::SubpassDescription()
		.setPipelineBindPoint(vk::PipelineBindPoint::eGraphics)
		.setColorAttachments(ColorAttachmentReferences)
		.setPDepthStencilAttachment(&DepthAttachmentReference);

	/***********************************************************************************
	VK_DEPENDENCY_BY_REGION_BIT specifies that dependencies will be framebuffer-local.

	VK_DEPENDENCY_VIEW_LOCAL_BIT specifies that a subpass has more than one view.

	VK_DEPENDENCY_DEVICE_GROUP_BIT specifies that dependencies are non-device-local dependency.
	************************************************************************************/

	auto SubpassDependency = vk::SubpassDependency()
		.setSrcSubpass(VK_SUBPASS_EXTERNAL)
		.setDstSubpass(0u)
		.setSrcStageMask(vk::PipelineStageFlagBits::eTopOfPipe)
		.setDstStageMask(vk::PipelineStageFlagBits::eBottomOfPipe)
		.setSrcAccessMask(vk::AccessFlagBits::eNoneKHR)
		.setDstAccessMask(vk::AccessFlagBits::eNoneKHR)
		.setDependencyFlags(vk::DependencyFlagBits::eByRegion);

	auto vkCreateInfo = vk::RenderPassCreateInfo()
		.setAttachments(AttachmentDescriptions)
		.setSubpassCount(1u)
		.setPSubpasses(&SubpassDescription)
		.setDependencyCount(1u)
		.setPDependencies(&SubpassDependency);

	VERIFY_VK(GetNativeDevice().createRenderPass(&vkCreateInfo, VK_ALLOCATION_CALLBACKS, &m_Native));
}

VulkanFramebuffer::VulkanFramebuffer(const VulkanDevice& Device, const RHIFrameBufferCreateInfo& CreateInfo)
	: VkHwResource(Device)
	, RHIFrameBuffer(CreateInfo)
{
	std::vector<vk::ImageView> Attachments;
	//for (auto& ColorAttachment : CreateInfo.ColorAttachments)
	//{
	//	if (ColorAttachment.Image)
	//	{
	//		auto Image = Cast<VulkanImage>(ColorAttachment.Image);
	//		assert(Image &&
	//			Image->GetWidth() == CreateInfo.Width &&
	//			Image->GetHeight() == CreateInfo.Height &&
	//			Image->GetArrayLayers() == CreateInfo.ArrayLayers);

	//		//Attachments.push_back(Image->GetOrCrateImageView(AllSubresource));
	//	}
	//}
	//if (CreateInfo.DepthStencilAttachment.Image)
	//{
	//	auto Image = Cast<VulkanImage>(CreateInfo.DepthStencilAttachment.Image);
	//	assert(Image);

	//	//Attachments.push_back(Image->GetOrCrateImageView(AllSubresource));
	//}

	auto vkCreateInfo = vk::FramebufferCreateInfo()
		//.setRenderPass(m_RenderPass)
		.setAttachments(Attachments)
		.setWidth(CreateInfo.Width)
		.setHeight(CreateInfo.Height)
		.setLayers(CreateInfo.ArrayLayers);

	VERIFY_VK(GetNativeDevice().createFramebuffer(&vkCreateInfo, VK_ALLOCATION_CALLBACKS, &m_Native));
}