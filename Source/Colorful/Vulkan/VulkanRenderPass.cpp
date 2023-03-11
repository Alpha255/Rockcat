#if 0
/*************************************************
	When defining a memory dependency, using only VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT or VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT would never
	make any accesses available and/or visible because these stages do not access memory.

	VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT and VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT are useful for accomplishing layout transitions and queue
	ownership operations when the required execution dependency is satisfied by other means - for example, semaphore operations between queues.


	A render pass represents a collection of attachments, subpasses, and dependencies between the subpasses,
	and describes how the attachments are used over the course of the subpasses.
	The use of a render pass in a command buffer is a render pass instance.

	A subpass represents a phase of rendering that reads and writes a subset of the attachments in a render pass.
	Rendering commands are recorded into a particular subpass of a render pass instance.

	A subpass description describes the subset of attachments that is involved in the execution of a subpass.
	Each subpass can read from some attachments as input attachments, write to some as color attachments or depth/stencil attachments,
	and perform multisample resolve operations to resolve attachments. A subpass description can also include a set of preserve attachments,
	which are attachments that are not read or written by the subpass but whose contents must be preserved throughout the subpass.

	The subpasses in a render pass all render to the same dimensions,
	and fragments for pixel (x,y,layer) in one subpass can only read attachment contents written by previous subpasses at that same (x,y,layer) location.

	Subpass dependencies describe execution and memory dependencies between subpasses.

	A render pass describes the structure of subpasses and attachments independent of any specific image views for the attachments.
	A render pass and a framebuffer define the complete render target state for one or more subpasses as well as the algorithmic dependencies between the subpasses.
	A render pass must have at least one subpass.
**************************************************/

VulkanRenderPass::VulkanRenderPass(const RenderPassDesc& desc)
{
	/// pPreserveAttachments is a pointer to an array of preserveAttachmentCount render pass attachment indices identifying attachments that are not used by this subpass, 
	/// but whose contents must be preserved throughout the subpass.

	assert(desc.AttachmentDescs.size() > 0u);

	std::vector<VkAttachmentDescription> attachmentDescs(desc.AttachmentDescs.size());
	for (uint32_t i = 0u; i < desc.AttachmentDescs.size(); ++i)
	{
		auto const& attachmentDesc = desc.AttachmentDescs[i];
		attachmentDescs[i] =
		{
			0u,
			VkEnumTranslator::format(attachmentDesc.Format),
			static_cast<VkSampleCountFlagBits>(attachmentDesc.SampleCount),
			VkEnumTranslator::attachmentLoadOp(attachmentDesc.LoadOp),
			VkEnumTranslator::attachmentStoreOp(attachmentDesc.StoreOp),
			VkEnumTranslator::attachmentLoadOp(attachmentDesc.StencilLoadOp),
			VkEnumTranslator::attachmentStoreOp(attachmentDesc.StencilStoreOp),
			VkEnumTranslator::imageLayout(attachmentDesc.InitialLayout),
			VkEnumTranslator::imageLayout(attachmentDesc.FinalLayout)
		};
	}

	uint32_t curAttachmentRef = 0u;
	std::vector<VkAttachmentReference> attachmentRefs;
	auto getAttachmentRefs = [&](size_t count, const RenderPassDesc::AttachmentReference* refs) -> const VkAttachmentReference* {
		if (count > 0u)
		{
			auto const* curAttachmentRefs = &attachmentRefs[curAttachmentRef];
			for (uint32_t i = 0u; i < count; ++i, ++curAttachmentRef)
			{
				auto const& ref = refs[i];
				assert(ref.Index < desc.AttachmentDescs.size());
				attachmentRefs.emplace_back(
					VkAttachmentReference
					{
						ref.Index,
						VkEnumTranslator::imageLayout(ref.Layout)
					});
			}

			return curAttachmentRefs;
		}

		return nullptr;
	};

	/**************************************************************************************************************
	pPreserveAttachments is a pointer to an array of preserveAttachmentCount render pass attachment indices identifying attachments that are not used by this subpass, 
	but whose contents must be preserved throughout the subpass.
	***************************************************************************************************************/

	std::vector<VkSubpassDescription> subpassDescs(desc.SubPasses.size());
	for (uint32_t i = 0u; i < desc.SubPasses.size(); ++i)
	{
		auto const& subpassDesc = desc.SubPasses[i];
		assert(subpassDesc.ResolveAttachments.size() == 0u || subpassDesc.ResolveAttachments.size() == subpassDesc.ColorAttachments.size());

		subpassDescs[i] =
		{
			0u,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			static_cast<uint32_t>(subpassDesc.InputAttachments.size()),
			getAttachmentRefs(subpassDesc.InputAttachments.size(), subpassDesc.InputAttachments.data()),
			static_cast<uint32_t>(subpassDesc.ColorAttachments.size()),
			getAttachmentRefs(subpassDesc.ColorAttachments.size(), subpassDesc.ColorAttachments.data()),
			getAttachmentRefs(subpassDesc.ResolveAttachments.size(), subpassDesc.ResolveAttachments.data()),
			getAttachmentRefs(1u, &subpassDesc.DepthStencilAttachment),
			static_cast<uint32_t>(subpassDesc.PreserveAttachments.size()),
			subpassDesc.PreserveAttachments.data()
		};
	}

	std::vector<VkSubpassDependency> subpassDependencies(desc.SubPassDependencies.size());
	for (uint32_t i = 0u; i < desc.SubPassDependencies.size(); ++i)
	{
		auto const& dependency = desc.SubPassDependencies[i];
		subpassDependencies[i] =
		{
			dependency.SrcSubpassIndex,
			dependency.DstSubpassIndex,
			VkEnumTranslator::pipelineStageFlags(dependency.SrcStageFlags),
			VkEnumTranslator::pipelineStageFlags(dependency.DstStageFlags),
			VkEnumTranslator::accessFlags(dependency.SrcAccessFlags),
			VkEnumTranslator::accessFlags(dependency.DstAccessFlags),
			VK_DEPENDENCY_BY_REGION_BIT /// ????? 
		};
	}

	VkRenderPassCreateInfo CreateInfo
	{
		VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
		nullptr,
		0u,
		static_cast<uint32_t>(attachmentDescs.size()),
		attachmentDescs.data(),
		static_cast<uint32_t>(subpassDescs.size()),
		subpassDescs.data(),
		static_cast<uint32_t>(subpassDependencies.size()),
		subpassDependencies.data()
	};

	VERIFY_VK(vkCreateRenderPass(s_Device->get(), &CreateInfo, VK_ALLOCATION_CALLBACKS, Reference()));
}

VulkanRenderPass::~VulkanRenderPass()
{
	vkDestroyRenderPass(s_Device->get(), get(), VK_ALLOCATION_CALLBACKS);
}

#endif

#include "Colorful/Vulkan/VulkanRenderPass.h"
#include "Colorful/Vulkan/VulkanDevice.h"

NAMESPACE_START(RHI)

VulkanFramebuffer::VulkanFramebuffer(VulkanDevice* Device, const FrameBufferDesc& Desc)
	: VkHWObject(Device, Desc.Width, Desc.Height, Desc.Depth, Desc)
{
	CreateRenderPass(Desc);

	std::vector<VkImageView> Attachments;
	for (auto& ColorAttachment : Desc.ColorAttachments)
	{
		if (ColorAttachment)
		{
			auto Image = std::static_pointer_cast<VulkanImage>(ColorAttachment);
			assert(Image &&
				Image->Width() == Desc.Width &&
				Image->Height() == Desc.Height &&
				Image->Depth() == Desc.Depth);

			Attachments.push_back(Image->GetOrCrateImageView(AllSubresource));
		}
	}
	if (Desc.DepthStencilAttachment)
	{
		auto Image = std::static_pointer_cast<VulkanImage>(Desc.DepthStencilAttachment);
		assert(Image);

		Attachments.push_back(Image->GetOrCrateImageView(AllSubresource));
	}

	VkFramebufferCreateInfo CreateInfo
	{
		VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
		nullptr,
		0u,
		m_RenderPass,
		static_cast<uint32_t>(Attachments.size()),
		Attachments.data(),
		Desc.Width,
		Desc.Height,
		Desc.Depth
	};

	VERIFY_VK(vkCreateFramebuffer(m_Device->Get(), &CreateInfo, VK_ALLOCATION_CALLBACKS, Reference()));
}

void VulkanFramebuffer::CreateRenderPass(const FrameBufferDesc& Desc)
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

	std::vector<VkAttachmentReference> ColorAttachmentReferences(Desc.ColorAttachments.size());
	std::vector<VkAttachmentDescription> AttachmentDescriptions(Desc.ColorAttachments.size());
	VkAttachmentReference DepthAttachmentReference;

	for (uint32_t ColorAttachmentIndex = 0u; ColorAttachmentIndex < Desc.ColorAttachments.size(); ++ColorAttachmentIndex)
	{
		AttachmentDescriptions[ColorAttachmentIndex].flags = 0u;
		AttachmentDescriptions[ColorAttachmentIndex].format = VkType::Format(Desc.ColorAttachments[ColorAttachmentIndex]->Format());
		AttachmentDescriptions[ColorAttachmentIndex].samples = VK_SAMPLE_COUNT_1_BIT;
		AttachmentDescriptions[ColorAttachmentIndex].loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
		AttachmentDescriptions[ColorAttachmentIndex].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		AttachmentDescriptions[ColorAttachmentIndex].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		AttachmentDescriptions[ColorAttachmentIndex].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		AttachmentDescriptions[ColorAttachmentIndex].initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		AttachmentDescriptions[ColorAttachmentIndex].finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		ColorAttachmentReferences[ColorAttachmentIndex].attachment = ColorAttachmentIndex;
		ColorAttachmentReferences[ColorAttachmentIndex].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	}

	if (Desc.DepthStencilAttachment)
	{
		AttachmentDescriptions.push_back(
			VkAttachmentDescription
			{
				0u,
				VkType::Format(Desc.DepthStencilAttachment->Format()),
				VK_SAMPLE_COUNT_1_BIT,
				VK_ATTACHMENT_LOAD_OP_LOAD,
				VK_ATTACHMENT_STORE_OP_STORE,
				VK_ATTACHMENT_LOAD_OP_DONT_CARE,
				VK_ATTACHMENT_STORE_OP_DONT_CARE,
				VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
				VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
			}
		);

		DepthAttachmentReference = VkAttachmentReference
		{
			static_cast<uint32_t>(AttachmentDescriptions.size()) - 1u,
			VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
		};
	}

	VkSubpassDescription SubpassDescription
	{
		0u,
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		0u,       /// inputAttachmentCount
		nullptr,  /// pInputAttachments
		static_cast<uint32_t>(ColorAttachmentReferences.size()),
		ColorAttachmentReferences.data(),
		nullptr,  /// pResolveAttachments
		Desc.DepthStencilAttachment ? &DepthAttachmentReference : nullptr,
		0u,       /// preserveAttachmentCount
		nullptr   /// pPreserveAttachments
	};

	/***********************************************************************************
	VK_DEPENDENCY_BY_REGION_BIT specifies that dependencies will be framebuffer-local.

	VK_DEPENDENCY_VIEW_LOCAL_BIT specifies that a subpass has more than one view.

	VK_DEPENDENCY_DEVICE_GROUP_BIT specifies that dependencies are non-device-local dependency.
	************************************************************************************/

	VkSubpassDependency SubpassDependency
	{
		VK_SUBPASS_EXTERNAL,
		0u,
		VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
		VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
		VK_ACCESS_NONE_KHR,
		VK_ACCESS_NONE_KHR,
		VK_DEPENDENCY_BY_REGION_BIT
	};

	VkRenderPassCreateInfo CreateInfo
	{
		VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
		nullptr,
		0u,
		static_cast<uint32_t>(AttachmentDescriptions.size()),
		AttachmentDescriptions.data(),
		1u,
		&SubpassDescription,
		0u,
		&SubpassDependency
	};

	VERIFY_VK(vkCreateRenderPass(m_Device->Get(), &CreateInfo, VK_ALLOCATION_CALLBACKS, &m_RenderPass));
}

VulkanFramebuffer::~VulkanFramebuffer()
{
	vkDestroyRenderPass(m_Device->Get(), m_RenderPass, VK_ALLOCATION_CALLBACKS);
	m_RenderPass = VK_NULL_HANDLE;

	vkDestroyFramebuffer(m_Device->Get(), Get(), VK_ALLOCATION_CALLBACKS);
}

NAMESPACE_END(RHI)