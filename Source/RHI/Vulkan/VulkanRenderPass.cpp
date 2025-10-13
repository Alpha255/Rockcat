#include "RHI/Vulkan/VulkanRenderPass.h"
#include "RHI/Vulkan/VulkanDevice.h"

VulkanRenderPass::VulkanRenderPass(const VulkanDevice& Device, const RHIRenderPassDesc& Desc)
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

	assert(Desc.ColorAttachments.size() > 0u);

	std::vector<vk::AttachmentReference> ColorAttachmentReferences(Desc.ColorAttachments.size());
	std::vector<vk::AttachmentDescription> AttachmentDescriptions(Desc.ColorAttachments.size());

	vk::AttachmentReference DepthAttachmentReference;

	auto SampleCount = GetSampleCount(Desc.SampleCount);
	for (uint32_t Index = 0u; Index < Desc.ColorAttachments.size(); ++Index)
	{
		auto& Attachment = Desc.ColorAttachments[Index];

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

	if (Desc.HasDepthStencil())
	{
		auto& Attachment = Desc.DepthStencilAttachment;

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

VulkanFrameBuffer::VulkanFrameBuffer(const VulkanDevice& Device, vk::RenderPass CompatibleRenderPass, const RHIFrameBufferDesc& Desc)
	: VkHwResource(Device)
	, RHIFrameBuffer(Desc)
{
	std::vector<vk::ImageView> Attachments;
	for (auto& Color : Desc.ColorAttachments)
	{
		assert(Color.Texture && 
			Color.GetWidth() == Desc.Width &&
			Color.GetHeight() == Desc.Height && 
			Color.GetArrayLayers() == Desc.ArrayLayers);

	//	if (ColorAttachment.Image)
	//	{
	//		auto Image = Cast<VulkanImage>(ColorAttachment.Image);
	//		assert(Image &&
	//			Image->GetWidth() == Desc.Width &&
	//			Image->GetHeight() == Desc.Height &&
	//			Image->GetArrayLayers() == Desc.ArrayLayers);

	//		//Attachments.push_back(Image->GetOrCrateImageView(AllSubresource));
	//	}
	//}
	//if (Desc.DepthStencilAttachment.Image)
	//{
	//	auto Image = Cast<VulkanImage>(Desc.DepthStencilAttachment.Image);
	//	assert(Image);

	//	//Attachments.push_back(Image->GetOrCrateImageView(AllSubresource));
	}

	if (Desc.HasDepthStencil())
	{
		assert(Desc.DepthStencilAttachment.GetWidth() == Desc.Width && 
			Desc.DepthStencilAttachment.GetHeight() == Desc.Height && 
			Desc.DepthStencilAttachment.GetArrayLayers() == Desc.ArrayLayers);
	}

	vk::FramebufferCreateInfo FrameBufferCreateInfo;
	FrameBufferCreateInfo.setRenderPass(CompatibleRenderPass)
		.setAttachments(Attachments)
		.setWidth(Desc.Width)
		.setHeight(Desc.Height)
		.setLayers(Desc.ArrayLayers);

	VERIFY_VK(GetNativeDevice().createFramebuffer(&FrameBufferCreateInfo, VK_ALLOCATION_CALLBACKS, &m_Native));
}