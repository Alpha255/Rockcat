#include "RHI/Vulkan/VulkanPipeline.h"
#include "RHI/Vulkan/VulkanDevice.h"
#include "RHI/Vulkan/VulkanLayerExtensions.h"
#include "RHI/Vulkan/VulkanShader.h"
#include "RHI/Vulkan/VulkanCommandListContext.h"
#include "Services/ShaderLibrary.h"

VulkanPipelineCache::VulkanPipelineCache(const VulkanDevice& Device)
	: VkHwResource(Device)
{
	vk::PipelineCacheCreateInfo Desc;
	VERIFY_VK(GetNativeDevice().createPipelineCache(&Desc, VK_ALLOCATION_CALLBACKS, &m_Native));
}

VulkanPipeline::VulkanPipeline(const VulkanDevice& Device)
	: VkHwResource(Device)
{
}

VulkanGraphicsPipeline::VulkanGraphicsPipeline(const VulkanDevice& Device, vk::PipelineCache PipelineCache, const RHIGraphicsPipelineDesc& Desc)
	: VulkanPipeline(Device)
	, RHIGraphicsPipeline(Desc)
{
	m_PipelineState = std::make_shared<VulkanPipelineState>(Device, Desc);

	std::vector<VulkanShader> Shaders;
	std::vector<vk::PipelineShaderStageCreateInfo> ShaderStageCreateInfos;
	for (auto& Shader : Desc.Shaders)
	{
		if (Shader)
		{
			auto Module = Cast<VulkanShader>(Shader->GetRHI(Device));
			assert(Module);

			ShaderStageCreateInfos.emplace_back(vk::PipelineShaderStageCreateInfo())
				.setStage(GetShaderStage(Shader->GetStage()))
				.setModule(Module->GetNative())
				.setPName(Shader->GetEntryPoint());
		}
	}
	assert(ShaderStageCreateInfos.size() > 0u);

	vk::PipelineRasterizationStateCreateInfo RasterizationStateCreateInfo;
	RasterizationStateCreateInfo.setDepthClampEnable(Desc.RasterizationState.EnableDepthClamp)
		.setRasterizerDiscardEnable(false)
		.setPolygonMode(GetPolygonMode(Desc.RasterizationState.PolygonMode))
		.setCullMode(GetCullMode(Desc.RasterizationState.CullMode))
		.setFrontFace(GetFrontFace(Desc.RasterizationState.FrontFace))
		.setDepthBiasEnable(Desc.RasterizationState.DepthBias != 0.0f)
		.setDepthBiasConstantFactor(Desc.RasterizationState.DepthBias)
		.setDepthBiasClamp(Desc.RasterizationState.DepthBiasClamp)
		.setDepthBiasSlopeFactor(Desc.RasterizationState.DepthBiasSlope)
		.setLineWidth(Desc.RasterizationState.LineWidth);

	vk::StencilOpState FrontFaceStencilState;
	FrontFaceStencilState.setFailOp(GetStencilOp(Desc.DepthStencilState.FrontFaceStencil.FailOp))
		.setPassOp(GetStencilOp(Desc.DepthStencilState.FrontFaceStencil.PassOp))
		.setDepthFailOp(GetStencilOp(Desc.DepthStencilState.FrontFaceStencil.DepthFailOp))
		.setCompareOp(GetCompareFunc(Desc.DepthStencilState.FrontFaceStencil.CompareFunc))
		.setCompareMask(Desc.DepthStencilState.StencilReadMask)
		.setWriteMask(Desc.DepthStencilState.StencilWriteMask)
		.setReference(Desc.DepthStencilState.FrontFaceStencil.Ref);
	vk::StencilOpState BackFaceStencilState;
	BackFaceStencilState.setFailOp(GetStencilOp(Desc.DepthStencilState.BackFaceStencil.FailOp))
		.setPassOp(GetStencilOp(Desc.DepthStencilState.BackFaceStencil.PassOp))
		.setDepthFailOp(GetStencilOp(Desc.DepthStencilState.BackFaceStencil.DepthFailOp))
		.setCompareOp(GetCompareFunc(Desc.DepthStencilState.BackFaceStencil.CompareFunc))
		.setCompareMask(Desc.DepthStencilState.StencilReadMask)
		.setWriteMask(Desc.DepthStencilState.StencilWriteMask)
		.setReference(Desc.DepthStencilState.BackFaceStencil.Ref);

	vk::PipelineDepthStencilStateCreateInfo DepthStencilStateCreateInfo;
	DepthStencilStateCreateInfo.setDepthTestEnable(Desc.DepthStencilState.EnableDepth)
		.setDepthWriteEnable(Desc.DepthStencilState.EnableDepthWrite)
		.setDepthCompareOp(GetCompareFunc(Desc.DepthStencilState.DepthCompareFunc))
		.setDepthBoundsTestEnable(Desc.DepthStencilState.EnableDepthBoundsTest)
		.setStencilTestEnable(Desc.DepthStencilState.EnableStencil)
		.setFront(FrontFaceStencilState)
		.setBack(BackFaceStencilState)
		.setMinDepthBounds(Desc.DepthStencilState.DepthBounds.x)
		.setMaxDepthBounds(Desc.DepthStencilState.DepthBounds.y);

	std::vector<vk::PipelineColorBlendAttachmentState> ColorBlendAttachmentStates;
	for (uint32_t AttachmentIndex = 0u; AttachmentIndex < ERHILimitations::MaxRenderTargets; ++AttachmentIndex)
	{
		auto& BlendState = Desc.BlendState.RenderTargetBlends[AttachmentIndex];

		ColorBlendAttachmentStates.emplace_back(vk::PipelineColorBlendAttachmentState())
			.setBlendEnable(BlendState.Enable)
			.setSrcColorBlendFactor(GetBlendFactor(BlendState.SrcColor))
			.setDstColorBlendFactor(GetBlendFactor(BlendState.DstColor))
			.setColorBlendOp(GetBlendOp(BlendState.ColorOp))
			.setSrcAlphaBlendFactor(GetBlendFactor(BlendState.SrcAlpha))
			.setDstAlphaBlendFactor(GetBlendFactor(BlendState.DstAlpha))
			.setAlphaBlendOp(GetBlendOp(BlendState.AlphaOp))
			.setColorWriteMask(GetColorComponentFlags(BlendState.ColorMask));
	}
	
	vk::PipelineColorBlendStateCreateInfo BlendStateCreateInfo;
	BlendStateCreateInfo.setLogicOpEnable(Desc.BlendState.EnableLogicOp)
		.setLogicOp(GetLogicOp(Desc.BlendState.LogicOp))
		.setAttachments(ColorBlendAttachmentStates);

	vk::PipelineTessellationStateCreateInfo TessellationStateCreateInfo;

	vk::PipelineMultisampleStateCreateInfo MultisampleStateCreateInfo;
	MultisampleStateCreateInfo.setRasterizationSamples(GetSampleCount(Desc.MultisampleState.SampleCount))
		.setSampleShadingEnable(Desc.MultisampleState.EnableSampleShading)
		.setMinSampleShading(Desc.MultisampleState.MinSampleShading)
		.setPSampleMask(Desc.MultisampleState.SampleMask)
		.setAlphaToCoverageEnable(Desc.MultisampleState.EnableAlphaToCoverage)
		.setAlphaToOneEnable(Desc.MultisampleState.EnableAlphaToOne);

	vk::PipelineViewportStateCreateInfo ViewportStateCreateInfo;

	std::vector<vk::DynamicState> DynamicStates
	{
		vk::DynamicState::eViewport,
		vk::DynamicState::eScissor
	};
	
	vk::PipelineDynamicStateCreateInfo DynamicStateCreateInfo;
	if (Device.GetExtensionSettings().DynamicState)
	{
		DynamicStateCreateInfo.setDynamicStates(DynamicStates);
	}
	else
	{
		assert(Desc.ScissorRects.size() && Desc.Viewports.size());

		std::vector<vk::Viewport> Viewports(Desc.Viewports.size());
		std::vector<vk::Rect2D> ScissorRects(Desc.ScissorRects.size());

		for (uint32_t Index = 0; Index < Desc.Viewports.size(); ++Index)
		{
			auto& Viewport = Desc.Viewports[Index];

			Viewports[Index].setX(Viewport.LeftTop.x)
				.setY(Viewport.LeftTop.y)
				.setWidth(Viewport.Extent.x)
				.setHeight(Viewport.Extent.y)
				.setMinDepth(Viewport.DepthRange.x)
				.setMaxDepth(Viewport.DepthRange.y);
		}

		for (uint32_t Index = 0u; Index < Desc.ScissorRects.size(); ++Index)
		{
			auto& Scissor = Desc.ScissorRects[Index];

			ScissorRects[Index].setExtent(vk::Extent2D(Scissor.Extent.x, Scissor.Extent.y))
				.setOffset(vk::Offset2D(Scissor.LeftTop.x, Scissor.LeftTop.y));
		}

		ViewportStateCreateInfo.setViewports(Viewports)
			.setScissors(ScissorRects);
	}

	VulkanInputLayout InputState(Desc.InputLayout);  /// TODO: Cache input layout ?

	vk::PipelineInputAssemblyStateCreateInfo InputAssemblyStateCreateInfo;
	InputAssemblyStateCreateInfo.setTopology(GetPrimitiveTopology(Desc.PrimitiveTopology))
		.setPrimitiveRestartEnable(false);

	vk::GraphicsPipelineCreateInfo PipelineCreateInfo;
	PipelineCreateInfo.setStages(ShaderStageCreateInfos)
		.setPVertexInputState(&InputState)
		.setPInputAssemblyState(&InputAssemblyStateCreateInfo)
		.setPTessellationState(&TessellationStateCreateInfo)
		.setPViewportState(&ViewportStateCreateInfo)
		.setPRasterizationState(&RasterizationStateCreateInfo)
		.setPMultisampleState(&MultisampleStateCreateInfo)
		.setPDepthStencilState(&DepthStencilStateCreateInfo)
		.setPColorBlendState(&BlendStateCreateInfo)
		.setPDynamicState(&DynamicStateCreateInfo)
		.setLayout(Cast<VulkanPipelineState>(m_PipelineState)->GetPipelineLayout());

	if (Device.GetExtensionSettings().DynamicRendering)
	{
		vk::PipelineRenderingCreateInfoKHR DynamicRenderingCreateInfo;
		PipelineCreateInfo.setPNext(&DynamicRenderingCreateInfo);
	}
	else
	{
		m_RenderPass = std::make_shared<VulkanRenderPass>(Device, Desc.RenderPass);
		PipelineCreateInfo.setRenderPass(m_RenderPass->GetNative());
	}

	VERIFY_VK(GetNativeDevice().createGraphicsPipelines(PipelineCache, 1u, &PipelineCreateInfo, VK_ALLOCATION_CALLBACKS, &m_Native));
}

VulkanPipelineState::VulkanPipelineState(const VulkanDevice& Device, const RHIGraphicsPipelineDesc& Desc)
	: VkBaseDeviceResource(Device)
	, RHIPipelineState(Desc)
{
	CreateLayouts(Device, Desc);
	InitWriteDescriptorSets(Desc);
}

VulkanPipelineState::VulkanPipelineState(const VulkanDevice& Device, const RHIComputePipelineDesc& Desc)
	: VkBaseDeviceResource(Device)
	, RHIPipelineState(Desc)
{
	assert(false); // TODO
}

void VulkanPipelineState::CreateLayouts(const VulkanDevice& Device, const RHIGraphicsPipelineDesc& Desc)
{
	m_DescriptorSetLayout = std::make_unique<VulkanDescriptorSetLayout>(Device, Desc);
	m_PipelineLayout = std::make_unique<VulkanPipelineLayout>(Device, m_DescriptorSetLayout->GetNative());
}

void VulkanPipelineState::InitWriteDescriptorSets(const RHIGraphicsPipelineDesc& Desc)
{
	for (uint32_t Index = 0u; Index < Desc.Shaders.size(); ++Index)
	{
		if (!Desc.Shaders[Index])
		{
			continue;
		}

		const Shader& Shader = *Desc.Shaders[Index];
		for (auto& Variable : Shader.GetVariables())
		{
			auto& Write = m_Writes.emplace_back(vk::WriteDescriptorSet());
			Write.setDstSet(m_Set)
				.setDescriptorCount(1u)
				.setDescriptorType(GetDescriptorType(Variable.Type))
				.setDstBinding(Variable.Binding);

			switch (Variable.Type)
			{
			case ERHIResourceType::UniformTexelBuffer:
			case ERHIResourceType::StorageTexelBuffer:
			case ERHIResourceType::UniformBuffer:
			case ERHIResourceType::StorageBuffer:
			case ERHIResourceType::UniformBufferDynamic:
			case ERHIResourceType::StorageBufferDynamic:
			{
				size_t DescriptorIndex = m_BufferInfos.size();

				m_BufferInfos.emplace_back(vk::DescriptorBufferInfo())
					.setBuffer(nullptr)
					.setOffset(0u)
					.setRange(0u);
				Write.setPBufferInfo(&m_BufferInfos.back());

				Variable.BindOnSet([this, DescriptorIndex](const RHIResource* Buffer) {
					auto VkBuffer = Cast<VulkanBuffer>(Buffer);
					assert(VkBuffer);
					auto& Info = m_BufferInfos[DescriptorIndex];
					if (SetDirty(Info.buffer != VkBuffer->GetNative()))
					{
						Info.setBuffer(VkBuffer->GetNative())
							.setOffset(0u)
							.setRange(VkBuffer->GetSize());
					}
				});
				break;
			}
			case ERHIResourceType::SampledImage:
			case ERHIResourceType::StorageImage:
			{
				size_t DescriptorIndex = m_ImageInfos.size();

				m_ImageInfos.emplace_back(vk::DescriptorImageInfo())
					.setImageView(nullptr)
					.setImageLayout(vk::ImageLayout::eUndefined)
					.setSampler(nullptr);
				Write.setPImageInfo(&m_ImageInfos.back());

				Variable.BindOnSet([this, DescriptorIndex](const RHIResource* Texture) {
					auto VkTexture = Cast<VulkanTexture>(Texture);
					assert(VkTexture);
					auto& Info = m_ImageInfos[DescriptorIndex];
				});
				break;
			}
			case ERHIResourceType::CombinedImageSampler:
			{
				size_t DescriptorIndex = m_ImageInfos.size();

				m_ImageInfos.emplace_back(vk::DescriptorImageInfo())
					.setImageView(nullptr)
					.setImageLayout(vk::ImageLayout::eUndefined)
					.setSampler(nullptr);
				Write.setPImageInfo(&m_ImageInfos.back());

				Variable.BindOnSetCombinedTextureSampler([this, DescriptorIndex](const RHITexture* Texture, const RHISampler* Sampler) {
					});

				break;
			}
			case ERHIResourceType::Sampler:
			{
				size_t DescriptorIndex = m_ImageInfos.size();

				m_ImageInfos.emplace_back(vk::DescriptorImageInfo())
					.setImageView(nullptr)
					.setImageLayout(vk::ImageLayout::eUndefined)
					.setSampler(nullptr);
				Write.setPImageInfo(&m_ImageInfos.back());

				Variable.BindOnSet([this, DescriptorIndex](const RHIResource* Sampler) {
					auto VkSampler = Cast<VulkanSampler>(Sampler);
					assert(VkSampler);
					auto& Info = m_ImageInfos[DescriptorIndex];
					if (SetDirty(Info.sampler != VkSampler->GetNative()))
					{
						Info.setSampler(VkSampler->GetNative());
					}
				});
				break;
			}
			default:
				assert(false);
				break;
			}
		}
	}
}

VulkanComputePipeline::VulkanComputePipeline(const VulkanDevice& Device, vk::PipelineCache PipelineCache, const RHIComputePipelineDesc& Desc)
	: VulkanPipeline(Device)
	, RHIComputePipeline(Desc)
{
	assert(Desc.ComputeShader);

	m_PipelineState = std::make_shared<VulkanPipelineState>(Device, Desc);

	auto Module = Cast<VulkanShader>(Desc.ComputeShader->GetRHI(Device));
	assert(Module);

	vk::PipelineShaderStageCreateInfo ShaderStageCreateInfo;
	ShaderStageCreateInfo.setModule(Module->GetNative())
		.setStage(vk::ShaderStageFlagBits::eCompute)
		.setPName(Desc.ComputeShader->GetEntryPoint());

	vk::ComputePipelineCreateInfo PipelineCreateInfo;
	PipelineCreateInfo.setStage(ShaderStageCreateInfo)
		.setLayout(Cast<VulkanPipelineState>(m_PipelineState)->GetPipelineLayout());

	VERIFY_VK(GetNativeDevice().createComputePipelines(PipelineCache, 1u, &PipelineCreateInfo, VK_ALLOCATION_CALLBACKS, &m_Native));
}
