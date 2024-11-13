#if 0
VulkanRenderPassPtr VkRenderStateCache::getOrCreateRenderPass(const RenderPassDesc& Desc)
{
	for (auto& pair : m_RenderPassCache)
	{
		if (pair.first == CreateInfo.hash())
		{
			return pair.second;
		}
	}

	if (CreateInfo.AttachmentDescs.size() == 0u)
	{
		RenderPassDesc defaultRenderPassDesc;
		defaultRenderPassDesc.AttachmentDescs.resize(2u);
		defaultRenderPassDesc.AttachmentDescs[0] = 
		{
			FormatAttribute::attribute_Vk(m_Swapchain->colorFormat()).Format,
			1u,
			RenderPassDesc::EAttachmentLoadOp::Clear,
			RenderPassDesc::EAttachmentStoreOp::DontCare,
			RenderPassDesc::EAttachmentLoadOp::Clear,
			RenderPassDesc::EAttachmentStoreOp::DontCare,
			Texture::EImageLayout::Undefined,
			Texture::EImageLayout::Present
		};

		defaultRenderPassDesc.AttachmentDescs[1] =
		{
			FormatAttribute::attribute_Vk(m_Swapchain->depthStencilFormat()).Format,
			1u,
			RenderPassDesc::EAttachmentLoadOp::Clear,
			RenderPassDesc::EAttachmentStoreOp::DontCare,
			RenderPassDesc::EAttachmentLoadOp::Clear,
			RenderPassDesc::EAttachmentStoreOp::DontCare,
			Texture::EImageLayout::Undefined,
			Texture::EImageLayout::DepthStencilAttachment
		};

		defaultRenderPassDesc.SubPasses.resize(1u);
		defaultRenderPassDesc.SubPasses[0] =
		{
			{},
			{ 
				{
					0u,
					Texture::EImageLayout::ColorAttachment
				} 
			},
			{},
			{},
			{
				1u,
				Texture::EImageLayout::DepthStencilAttachment
			}
		};

		defaultRenderPassDesc.SubPassDependencies.resize(2u);
		defaultRenderPassDesc.SubPassDependencies[0] =
		{
			VK_SUBPASS_EXTERNAL,
			0u,
			RenderPassDesc::EPipelineStageFlags::BottomOfPipe,
			RenderPassDesc::EPipelineStageFlags::ColorAttachmentOutput,
			RenderPassDesc::EAccessFlags::MemoryRead,
			RenderPassDesc::EAccessFlags::ColorAttachmentRead | RenderPassDesc::EAccessFlags::ColorAttachmentWrite,
		};

		defaultRenderPassDesc.SubPassDependencies[1] =
		{
			0u,
			VK_SUBPASS_EXTERNAL,
			RenderPassDesc::EPipelineStageFlags::ColorAttachmentOutput,
			RenderPassDesc::EPipelineStageFlags::BottomOfPipe,
			RenderPassDesc::EAccessFlags::ColorAttachmentRead | RenderPassDesc::EAccessFlags::ColorAttachmentWrite,
			RenderPassDesc::EAccessFlags::MemoryRead,
		};

		auto renderPass = create_shared<VulkanRenderPass>(defaultRenderPassDesc);
		m_RenderPassCache.push_back(std::make_pair(CreateInfo.hash(), renderPass));
		return renderPass;
	}

	auto renderPass = create_shared<VulkanRenderPass>(Desc);
	m_RenderPassCache.push_back(std::make_pair(CreateInfo.hash(), renderPass));
	return renderPass;
}
#else
#include "RHI/Vulkan/VulkanPipeline.h"
#include "RHI/Vulkan/VulkanDevice.h"
#include "RHI/Vulkan/VulkanRHI.h"
#include "RHI/Vulkan/VulkanLayerExtensions.h"
#include "RHI/Vulkan/VulkanShader.h"
#include "Engine/Services/SpdLogService.h"

VulkanPipelineCache::VulkanPipelineCache(const VulkanDevice& Device)
	: VkHwResource(Device)
{
	vk::PipelineCacheCreateInfo vkCreateInfo;
	VERIFY_VK(GetNativeDevice().createPipelineCache(&vkCreateInfo, VK_ALLOCATION_CALLBACKS, &m_Native));
}

VulkanPipeline::VulkanPipeline(const VulkanDevice& Device)
	: VkHwResource(Device)
{
}

VulkanGraphicsPipeline::VulkanGraphicsPipeline(const VulkanDevice& Device, vk::PipelineCache PipelineCache, const RHIGraphicsPipelineCreateInfo& CreateInfo)
	: VulkanPipeline(Device)
{
	std::vector<VulkanShader> Shaders;
	std::vector<vk::PipelineShaderStageCreateInfo> ShaderStageCreateInfos;
	for (auto& ShaderCreateInfo : CreateInfo.Shaders)
	{
		if (ShaderCreateInfo.Binary)
		{
			auto& ShaderModule = Shaders.emplace_back(VulkanShader(Device, ShaderCreateInfo));
			auto ShaderStageCreateInfo = vk::PipelineShaderStageCreateInfo()
				.setStage(GetShaderStage(ShaderCreateInfo.Stage))
				.setModule(ShaderModule.GetNative())
				.setPName("main");
			ShaderStageCreateInfos.emplace_back(std::move(ShaderStageCreateInfo));
		}
	}
	assert(ShaderStageCreateInfos.size() > 0u);

	auto RasterizationStateCreateInfo = vk::PipelineRasterizationStateCreateInfo()
		.setDepthClampEnable(CreateInfo.RasterizationState.EnableDepthClamp)
		.setRasterizerDiscardEnable(false)
		.setPolygonMode(GetPolygonMode(CreateInfo.RasterizationState.PolygonMode))
		.setCullMode(GetCullMode(CreateInfo.RasterizationState.CullMode))
		.setFrontFace(GetFrontFace(CreateInfo.RasterizationState.FrontFace))
		.setDepthBiasEnable(CreateInfo.RasterizationState.DepthBias != 0.0f)
		.setDepthBiasConstantFactor(CreateInfo.RasterizationState.DepthBias)
		.setDepthBiasClamp(CreateInfo.RasterizationState.DepthBiasClamp)
		.setDepthBiasSlopeFactor(CreateInfo.RasterizationState.DepthBiasSlope)
		.setLineWidth(CreateInfo.RasterizationState.LineWidth);

	auto FrontFaceStencilState = vk::StencilOpState()
		.setFailOp(GetStencilOp(CreateInfo.DepthStencilState.FrontFaceStencil.FailOp))
		.setPassOp(GetStencilOp(CreateInfo.DepthStencilState.FrontFaceStencil.PassOp))
		.setDepthFailOp(GetStencilOp(CreateInfo.DepthStencilState.FrontFaceStencil.DepthFailOp))
		.setCompareOp(GetCompareFunc(CreateInfo.DepthStencilState.FrontFaceStencil.CompareFunc))
		.setCompareMask(CreateInfo.DepthStencilState.StencilReadMask)
		.setWriteMask(CreateInfo.DepthStencilState.StencilWriteMask)
		.setReference(CreateInfo.DepthStencilState.FrontFaceStencil.Ref);
	auto BackFaceStencilState = vk::StencilOpState()
		.setFailOp(GetStencilOp(CreateInfo.DepthStencilState.BackFaceStencil.FailOp))
		.setPassOp(GetStencilOp(CreateInfo.DepthStencilState.BackFaceStencil.PassOp))
		.setDepthFailOp(GetStencilOp(CreateInfo.DepthStencilState.BackFaceStencil.DepthFailOp))
		.setCompareOp(GetCompareFunc(CreateInfo.DepthStencilState.BackFaceStencil.CompareFunc))
		.setCompareMask(CreateInfo.DepthStencilState.StencilReadMask)
		.setWriteMask(CreateInfo.DepthStencilState.StencilWriteMask)
		.setReference(CreateInfo.DepthStencilState.BackFaceStencil.Ref);

	auto DepthStencilStateCreateInfo = vk::PipelineDepthStencilStateCreateInfo()
		.setDepthTestEnable(CreateInfo.DepthStencilState.EnableDepth)
		.setDepthWriteEnable(CreateInfo.DepthStencilState.EnableDepthWrite)
		.setDepthCompareOp(GetCompareFunc(CreateInfo.DepthStencilState.DepthCompareFunc))
		.setDepthBoundsTestEnable(CreateInfo.DepthStencilState.EnableDepthBoundsTest)
		.setStencilTestEnable(CreateInfo.DepthStencilState.EnableStencil)
		.setFront(FrontFaceStencilState)
		.setBack(BackFaceStencilState)
		.setMinDepthBounds(CreateInfo.DepthStencilState.DepthBounds.x)
		.setMaxDepthBounds(CreateInfo.DepthStencilState.DepthBounds.y);

	std::vector<vk::PipelineColorBlendAttachmentState> ColorBlendAttachmentStates;
	for (uint32_t AttachmentIndex = 0u; AttachmentIndex < ERHILimitations::MaxRenderTargets; ++AttachmentIndex)
	{
		auto& BlendState = CreateInfo.BlendState.RenderTargetBlends[AttachmentIndex];

		ColorBlendAttachmentStates.emplace_back(vk::PipelineColorBlendAttachmentState()
			.setBlendEnable(BlendState.Enable)
			.setSrcColorBlendFactor(GetBlendFactor(BlendState.SrcColor))
			.setDstColorBlendFactor(GetBlendFactor(BlendState.DstColor))
			.setColorBlendOp(GetBlendOp(BlendState.ColorOp))
			.setSrcAlphaBlendFactor(GetBlendFactor(BlendState.SrcAlpha))
			.setDstAlphaBlendFactor(GetBlendFactor(BlendState.DstAlpha))
			.setAlphaBlendOp(GetBlendOp(BlendState.AlphaOp))
			.setColorWriteMask(GetColorComponentFlags(BlendState.ColorMask)));
	}
	auto BlendStateCreateInfo = vk::PipelineColorBlendStateCreateInfo()
		.setLogicOpEnable(CreateInfo.BlendState.EnableLogicOp)
		.setLogicOp(GetLogicOp(CreateInfo.BlendState.LogicOp))
		.setAttachments(ColorBlendAttachmentStates);

	vk::PipelineTessellationStateCreateInfo TessellationStateCreateInfo;

	auto ViewportStateCreateInfo = vk::PipelineViewportStateCreateInfo()
		.setScissorCount(1u)
		.setViewportCount(1u);

	auto MultisampleStateCreateInfo = vk::PipelineMultisampleStateCreateInfo()
		.setRasterizationSamples(GetSampleCount(CreateInfo.MultisampleState.SampleCount))
		.setSampleShadingEnable(CreateInfo.MultisampleState.EnableSampleShading)
		.setMinSampleShading(CreateInfo.MultisampleState.MinSampleShading)
		.setPSampleMask(CreateInfo.MultisampleState.SampleMask)
		.setAlphaToCoverageEnable(CreateInfo.MultisampleState.EnableAlphaToCoverage)
		.setAlphaToOneEnable(CreateInfo.MultisampleState.EnableAlphaToOne);

	/// Set viewport/scissor dynamic
	std::vector<vk::DynamicState> DynamicStates
	{
		vk::DynamicState::eViewport,
		vk::DynamicState::eScissor
	};

	/// Provided by VK_VERSION_1_3
	/// VK_DYNAMIC_STATE_CULL_MODE specifies that the cullMode state in VkPipelineRasterizationStateCreateInfo will be ignored 
	/// and must be set dynamically with vkCmdSetCullMode before any drawing commands.
	if (VulkanRHI::GetLayerExtensionConfigs().HasDynamicState)
	{
		DynamicStates.push_back(vk::DynamicState::eCullModeEXT);
		DynamicStates.push_back(vk::DynamicState::eFrontFaceEXT);
		DynamicStates.push_back(vk::DynamicState::ePrimitiveTopologyEXT);
	}

	auto DynamicStateCreateInfo = vk::PipelineDynamicStateCreateInfo()
		.setDynamicStates(DynamicStates);

	auto InputState = VulkanInputLayout(CreateInfo.InputLayout);
	
	auto InputAssemblyStateCreateInfo = vk::PipelineInputAssemblyStateCreateInfo()
		.setTopology(GetPrimitiveTopology(CreateInfo.PrimitiveTopology))
		.setPrimitiveRestartEnable(false);

	auto vkCreateInfo = vk::GraphicsPipelineCreateInfo()
		.setStages(ShaderStageCreateInfos)
		.setPVertexInputState(&InputState)
		.setPInputAssemblyState(&InputAssemblyStateCreateInfo)
		.setPTessellationState(&TessellationStateCreateInfo)
		.setPViewportState(&ViewportStateCreateInfo)
		.setPRasterizationState(&RasterizationStateCreateInfo)
		.setPMultisampleState(&MultisampleStateCreateInfo)
		.setPDepthStencilState(&DepthStencilStateCreateInfo)
		.setPColorBlendState(&BlendStateCreateInfo)
		.setPDynamicState(&DynamicStateCreateInfo)
		.setLayout(nullptr)
		.setRenderPass(nullptr);

	/// #TODO Batch creations ???
	VERIFY_VK(GetNativeDevice().createGraphicsPipelines(PipelineCache, 1u, &vkCreateInfo, VK_ALLOCATION_CALLBACKS, &m_Native));
}

#if 0
VulkanGraphicsPipelineState::VulkanGraphicsPipelineState(const GraphicsPipelineDesc& Desc, VkDescriptorSet DescriptorSet)
	: PipelineState(Desc)
	, m_DescriptorSet(DescriptorSet)
{
	assert(m_DescriptorSet != VK_NULL_HANDLE);

	CreateInfo.Shaders.ForEach([this, DescriptorSet](const IShader* Shader) {
		for (auto& Variable : Shader->Desc()->Variables)
		{
			VkWriteDescriptorSet WriteDescriptorSet
			{
				VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
				nullptr,
				DescriptorSet,
				Variable.Binding,
				0u,
				1u,
				VkType::DescriptorType(Variable.Type)
			};

			if (Variable.Type == EResourceType::Sampler ||
				Variable.Type == EResourceType::CombinedImageSampler ||
				Variable.Type == EResourceType::SampledImage ||
				Variable.Type == EResourceType::StorageImage)
			{
				m_ImageInfos.emplace_back(
					VkDescriptorImageInfo
					{
						VK_NULL_HANDLE,
						VK_NULL_HANDLE,
						VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL /// #TODO
					});
				WriteDescriptorSet.pImageInfo = &m_ImageInfos.back();
				m_Writes.emplace_back(WriteDescriptorSet);
			}
			else if (Variable.Type == EResourceType::UniformBuffer)
			{
				m_BufferInfos.emplace_back(
					VkDescriptorBufferInfo
					{
						VK_NULL_HANDLE,
						0u,
						VK_WHOLE_SIZE  /// #TODO
					});
				WriteDescriptorSet.pBufferInfo = &m_BufferInfos.back();
				m_Writes.emplace_back(WriteDescriptorSet);
			}
		}
	});
}

void VulkanGraphicsPipelineState::WriteImage(EShaderStage, uint32_t Binding, IImage* Image)
{
	auto VkImage = static_cast<VulkanImage*>(Image);
	const_cast<VkDescriptorImageInfo*>(m_Writes[Binding].pImageInfo)->imageView = VkImage->GetOrCrateImageView(AllSubresource);  /// #TODO

	VkImage->RequiredState = EResourceState::ShaderResource;
	ResourcesNeedTransitionState.emplace_back(ResourceNeedTransitionState{VkImage, nullptr});
}

void VulkanGraphicsPipelineState::WriteSampler(EShaderStage, uint32_t Binding, ISampler* Sampler)
{
	const_cast<VkDescriptorImageInfo*>(m_Writes[Binding].pImageInfo)
		->sampler = static_cast<VulkanSampler*>(Sampler)->Get();
}

void VulkanGraphicsPipelineState::WriteUniformBuffer(EShaderStage, uint32_t Binding, IBuffer* Buffer)
{
	auto VkBuffer = static_cast<VulkanBuffer*>(Buffer);
	const_cast<VkDescriptorBufferInfo*>(m_Writes[Binding].pBufferInfo)->buffer = VkBuffer->Get();

	VkBuffer->RequiredState = EResourceState::UniformBuffer;
	ResourcesNeedTransitionState.emplace_back(ResourceNeedTransitionState{ nullptr, VkBuffer });
}
#endif

#endif
