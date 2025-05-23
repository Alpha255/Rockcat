#include "RHI/Vulkan/VulkanRenderPass.h"
#include "RHI/Vulkan/VulkanDevice.h"

VulkanRenderPass::VulkanRenderPass(const VulkanDevice& Device, const RHIRenderPassCreateInfo& CreateInfo)
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

	auto SampleCount = GetSampleCount(CreateInfo.SampleCount);
	for (uint32_t Index = 0u; Index < CreateInfo.ColorAttachments.size(); ++Index)
	{
		auto& Attachment = CreateInfo.ColorAttachments[Index];

		AttachmentDescriptions[Index]
			.setFormat(GetFormat(Attachment.Format))
			.setSamples(SampleCount)
			.setLoadOp(GetAttachmentLoadOp(Attachment.LoadOp))
			.setStoreOp(GetAttachmentStoreOp(Attachment.StoreOp))
			.setStencilLoadOp(GetAttachmentLoadOp(Attachment.StencilLoadOp))
			.setStencilStoreOp(GetAttachmentStoreOp(Attachment.StencilStoreOp))
			.setInitialLayout(vk::ImageLayout::eColorAttachmentOptimal)
			.setFinalLayout(vk::ImageLayout::eColorAttachmentOptimal);

		ColorAttachmentReferences[Index]
			.setAttachment(Index)
			.setLayout(vk::ImageLayout::eAttachmentOptimal);
	}

	if (CreateInfo.HasDepthStencil())
	{
		auto& Attachment = CreateInfo.DepthStencilAttachment;

		AttachmentDescriptions.emplace_back(vk::AttachmentDescription())
			.setFormat(GetFormat(Attachment.Format))
			.setSamples(SampleCount)
			.setLoadOp(GetAttachmentLoadOp(Attachment.LoadOp))
			.setStoreOp(GetAttachmentStoreOp(Attachment.StoreOp))
			.setStencilLoadOp(GetAttachmentLoadOp(Attachment.StencilLoadOp))
			.setStencilStoreOp(GetAttachmentStoreOp(Attachment.StencilStoreOp))
			.setInitialLayout(vk::ImageLayout::eStencilAttachmentOptimal)
			.setFinalLayout(vk::ImageLayout::eStencilAttachmentOptimal);

		DepthAttachmentReference.setAttachment(static_cast<uint32_t>(AttachmentDescriptions.size()) - 1u)
			.setLayout(vk::ImageLayout::eStencilAttachmentOptimal);
	}

	vk::SubpassDescription SubpassDescription;
	SubpassDescription.setPipelineBindPoint(vk::PipelineBindPoint::eGraphics)
		.setColorAttachments(ColorAttachmentReferences)
		.setPDepthStencilAttachment(&DepthAttachmentReference);

	/***********************************************************************************
	VK_DEPENDENCY_BY_REGION_BIT specifies that dependencies will be framebuffer-local.

	VK_DEPENDENCY_VIEW_LOCAL_BIT specifies that a subpass has more than one view.

	VK_DEPENDENCY_DEVICE_GROUP_BIT specifies that dependencies are non-device-local dependency.
	************************************************************************************/

	vk::SubpassDependency SubpassDependency;
	SubpassDependency.setSrcSubpass(VK_SUBPASS_EXTERNAL)
		.setDstSubpass(0u)
		.setSrcStageMask(vk::PipelineStageFlagBits::eTopOfPipe)
		.setDstStageMask(vk::PipelineStageFlagBits::eBottomOfPipe)
		.setSrcAccessMask(vk::AccessFlagBits::eNoneKHR)
		.setDstAccessMask(vk::AccessFlagBits::eNoneKHR)
		.setDependencyFlags(vk::DependencyFlagBits::eByRegion);

	vk::RenderPassCreateInfo RenderPassCreateInfo;
	RenderPassCreateInfo.setAttachments(AttachmentDescriptions)
		.setSubpassCount(1u)
		.setPSubpasses(&SubpassDescription)
		.setDependencyCount(1u)
		.setPDependencies(&SubpassDependency);

	VERIFY_VK(GetNativeDevice().createRenderPass(&RenderPassCreateInfo, VK_ALLOCATION_CALLBACKS, &m_Native));
}

VulkanFramebuffer::VulkanFramebuffer(const VulkanDevice& Device, vk::RenderPass CompatibleRenderPass, const RHIFrameBufferCreateInfo& CreateInfo)
	: VkHwResource(Device)
	, RHIFrameBuffer(CreateInfo)
{
	std::vector<vk::ImageView> Attachments;
	for (auto& Color : CreateInfo.ColorAttachments)
	{
		assert(Color.Texture && 
			Color.GetWidth() == CreateInfo.Width &&
			Color.GetHeight() == CreateInfo.Height && 
			Color.GetArrayLayers() == CreateInfo.ArrayLayers);

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
	}

	if (CreateInfo.HasDepthStencil())
	{
		assert(CreateInfo.DepthStencilAttachment.GetWidth() == CreateInfo.Width && 
			CreateInfo.DepthStencilAttachment.GetHeight() == CreateInfo.Height && 
			CreateInfo.DepthStencilAttachment.GetArrayLayers() == CreateInfo.ArrayLayers);
	}

	vk::FramebufferCreateInfo FrameBufferCreateInfo;
	FrameBufferCreateInfo.setRenderPass(CompatibleRenderPass)
		.setAttachments(Attachments)
		.setWidth(CreateInfo.Width)
		.setHeight(CreateInfo.Height)
		.setLayers(CreateInfo.ArrayLayers);

	VERIFY_VK(GetNativeDevice().createFramebuffer(&FrameBufferCreateInfo, VK_ALLOCATION_CALLBACKS, &m_Native));
}