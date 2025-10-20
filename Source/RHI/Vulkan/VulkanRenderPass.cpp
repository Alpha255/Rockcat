#include "RHI/Vulkan/VulkanRenderPass.h"
#include "RHI/Vulkan/VulkanDevice.h"

VulkanRenderPass::VulkanRenderPass(const VulkanDevice& Device, const RHIRenderPassDesc& Desc)
	: VkHwResource(Device)
{
	// https://docs.vulkan.org/guide/latest/deprecated.html#render_pass_functions_replacement
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

VulkanRenderPass::VulkanRenderPass(const VulkanDevice& Device, const RHIRenderPassDesc2& Desc)
	: VkHwResource(Device)
{
	assert(Desc.RenderTargetLayout.IsValid());

	const size_t NumAttachments = Desc.RenderTargetLayout.Colors.size() + (Desc.RenderTargetLayout.HasDepthStencil() ? 1u : 0u);
	std::vector<vk::AttachmentDescription> AttachmentDescriptions(NumAttachments);

	for (auto& Color : Desc.RenderTargetLayout.Colors)
	{
		AttachmentDescriptions.emplace_back(vk::AttachmentDescription())
			.setFormat(GetFormat(Color.Format))
			.setSamples(GetSampleCount(Color.NumSamples))
			.setLoadOp(GetAttachmentLoadOp(Color.LoadOp))
			.setStoreOp(GetAttachmentStoreOp(Color.StoreOp))
			.setStencilLoadOp(GetAttachmentLoadOp(Color.StencilLoadOp))
			.setStencilStoreOp(GetAttachmentStoreOp(Color.StencilStoreOp))
			.setInitialLayout(GetImageLayout(Color.InitialStates))
			.setFinalLayout(GetImageLayout(Color.FinalStates));
	}

	if (Desc.RenderTargetLayout.HasDepthStencil())
	{
		auto& DepthStencil = Desc.RenderTargetLayout.DepthStencil;
		AttachmentDescriptions.emplace_back(vk::AttachmentDescription())
			.setFormat(GetFormat(DepthStencil.Format))
			.setSamples(GetSampleCount(DepthStencil.NumSamples))
			.setLoadOp(GetAttachmentLoadOp(DepthStencil.LoadOp))
			.setStoreOp(GetAttachmentStoreOp(DepthStencil.StoreOp))
			.setStencilLoadOp(GetAttachmentLoadOp(DepthStencil.StencilLoadOp))
			.setStencilStoreOp(GetAttachmentStoreOp(DepthStencil.StencilStoreOp))
			.setInitialLayout(GetImageLayout(Desc.RenderTargetLayout.DepthStencil.InitialStates))
			.setFinalLayout(GetImageLayout(Desc.RenderTargetLayout.DepthStencil.FinalStates));
	}

	std::vector<vk::AttachmentReference> AttachmentRefs;

	std::vector<vk::SubpassDependency> SubpassDependencies;

	vk::RenderPassCreateInfo CreateInfo;

	const size_t NumSubpasses = Desc.Subpasses.empty() ? 1u : Desc.Subpasses.size();
	std::vector<vk::SubpassDescription> SubpassDescriptions(NumSubpasses);
	if (Desc.Subpasses.empty())
	{
		// Create a default subpass
		auto& SubpassDescription = SubpassDescriptions[0];

		for (uint32_t Index = 0u; Index < Desc.RenderTargetLayout.Colors.size(); ++Index)
		{
			AttachmentRefs.emplace_back(vk::AttachmentReference(Index, vk::ImageLayout::eColorAttachmentOptimal));
		}

		SubpassDescription.setPipelineBindPoint(vk::PipelineBindPoint::eGraphics)
			.setColorAttachments(AttachmentRefs);

		if (Desc.RenderTargetLayout.HasDepthStencil())
		{
			AttachmentRefs.emplace_back(vk::AttachmentReference())
				.setAttachment(static_cast<uint32_t>(AttachmentDescriptions.size()) - 1u)
				.setLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);

			SubpassDescription.setPDepthStencilAttachment(&AttachmentRefs.back());
		}
	}
	else
	{
		auto GetSubpassDependency = [](
			uint32_t SrcSubpassIndex,
			uint32_t DstSubpassIndex,
			const RHIRenderPassDesc2::RHISubpassDesc& SrcSubpass, 
			const RHIRenderPassDesc2::RHISubpassDesc& DstSubpass,
			vk::SubpassDependency& Dependency
			)
			{
				vk::ImageLayout Layout = vk::ImageLayout::eUndefined;
				vk::PipelineStageFlags SrcPipelineStages;
				vk::PipelineStageFlags DstPipelineStages;
				vk::AccessFlags SrcAccessMasks;
				vk::AccessFlags DstAccessMasks;

				Dependency.setSrcSubpass(SrcSubpassIndex)
					.setDstSubpass(DstSubpassIndex);
			};

		SubpassDependencies.resize(Desc.Subpasses.size() + 1u);

		GetSubpassDependency(VK_SUBPASS_EXTERNAL, 0u, Desc.Subpasses.back(), Desc.Subpasses[0], SubpassDependencies[0]);

		for (uint32_t Index = 0u; Index < Desc.Subpasses.size(); ++Index)
		{
			auto& Subpass = Desc.Subpasses[Index];
			auto& SubpassDescription = SubpassDescriptions[Index];

			const bool IsLastSubpass = (Index == Desc.Subpasses.size() - 1u);
			uint32_t SubpassIndex = Index;
			uint32_t NextSubpassIndex = IsLastSubpass ? VK_SUBPASS_EXTERNAL : (Index + 1u);
			auto& NextSubpass = IsLastSubpass ? Desc.Subpasses[0u] : Desc.Subpasses[Index + 1u];

			for (auto& Input : Subpass.Inputs)
			{
				AttachmentRefs.emplace_back(vk::AttachmentReference(Input.Index, GetImageLayout(Input.States)));
			}
			SubpassDescription.setInputAttachments(AttachmentRefs);

			const size_t ColorAttachmentIndex = AttachmentRefs.size();
			for (auto& Color : Subpass.OutputColors)
			{
				AttachmentRefs.emplace_back(vk::AttachmentReference(Color.Index, GetImageLayout(Color.States)));
			}
			SubpassDescription.setColorAttachmentCount(static_cast<uint32_t>(Subpass.OutputColors.size()))
				.setPColorAttachments(&AttachmentRefs[ColorAttachmentIndex]);

			if (Subpass.HasDepthStencil())
			{
				AttachmentRefs.emplace_back(vk::AttachmentReference(Subpass.DepthStencil.Index, GetImageLayout(Subpass.DepthStencil.States)));
				SubpassDescription.setPDepthStencilAttachment(&AttachmentRefs.back());
			}

			GetSubpassDependency(SubpassIndex, NextSubpassIndex, Subpass, NextSubpass, SubpassDependencies[Index]);
		}
	}

	CreateInfo.setAttachments(AttachmentDescriptions)
		.setSubpasses(SubpassDescriptions)
		.setDependencies(SubpassDependencies);

	VERIFY_VK(GetNativeDevice().createRenderPass(&CreateInfo, VK_ALLOCATION_CALLBACKS, &m_Native));
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