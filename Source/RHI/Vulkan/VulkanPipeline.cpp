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
	, RHIGraphicsPipeline(CreateInfo)
{
	std::vector<VulkanShader> Shaders;
	std::vector<vk::PipelineShaderStageCreateInfo> ShaderStageCreateInfos;
	for (auto Shader : CreateInfo.Shaders)
	{
		if (Shader)
		{
			auto ShaderCreateInfo = RHIShaderCreateInfo()
				.SetStage(Shader->GetStage())
				.SetShaderBinary(Shader->GetBinary(ERenderHardwareInterface::Vulkan))
				.SetName(Shader->GetName().string());

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

	m_PipelineState = std::make_shared<VulkanPipelineState>(Device, CreateInfo);
}

VulkanPipelineState::VulkanPipelineState(const VulkanDevice& Device, const RHIGraphicsPipelineCreateInfo& GfxPipelineCreateInfo)
	: VkBaseDeviceResource(Device)
	, RHIPipelineState(GfxPipelineCreateInfo)
{
	m_DescriptorSetLayout = std::make_unique<VulkanDescriptorSetLayout>(Device, GetShaderResourceLayout());
	m_PipelineLayout = std::make_unique<VulkanPipelineLayout>(Device, m_DescriptorSetLayout->GetNative());

	InitWrites();
}

void VulkanPipelineState::CommitShaderResources()
{
	for (auto& ShaderStageResources : GetShaderResourceLayout())
	{
		for (auto& ResourceInfo : ShaderStageResources)
		{
			switch (ResourceInfo.Type)
			{
			case ERHIResourceType::UniformTexelBuffer:
			case ERHIResourceType::StorageTexelBuffer:
			case ERHIResourceType::UniformBuffer:
			case ERHIResourceType::StorageBuffer:
			case ERHIResourceType::UniformBufferDynamic:
			case ERHIResourceType::StorageBufferDynamic:
				m_BufferInfos[ResourceInfo.DescriptorIndex].setBuffer(nullptr);
				break;
			case ERHIResourceType::SampledImage:
			case ERHIResourceType::StorageImage:
				m_ImageInfos[ResourceInfo.DescriptorIndex].setImageView(nullptr);
				break;
			case ERHIResourceType::CombinedImageSampler:
				m_ImageInfos[ResourceInfo.DescriptorIndex].setImageView(nullptr);
				break;
			case ERHIResourceType::Sampler:
				m_ImageInfos[ResourceInfo.DescriptorIndex].setSampler(nullptr);
				break;
			default:
				assert(false);
				break;
			}
		}
	}

	GetNativeDevice().updateDescriptorSets(static_cast<uint32_t>(m_Writes.size()), m_Writes.data(), 0u, nullptr);
}

void VulkanPipelineState::InitWrites()
{
	for (auto& ShaderStageResources : m_ShaderResourceLayout)
	{
		for (auto& ResourceInfo : ShaderStageResources)
		{
			auto& Write = m_Writes.emplace_back(vk::WriteDescriptorSet());
			Write.setDstSet(m_Set)
				.setDescriptorCount(1u)
				.setDescriptorType(GetDescriptorType(ResourceInfo.Type))
				.setDstBinding(ResourceInfo.Binding);

			switch (ResourceInfo.Type)
			{
			case ERHIResourceType::UniformTexelBuffer:
			case ERHIResourceType::StorageTexelBuffer:
			case ERHIResourceType::UniformBuffer:
			case ERHIResourceType::StorageBuffer:
			case ERHIResourceType::UniformBufferDynamic:
			case ERHIResourceType::StorageBufferDynamic:
				ResourceInfo.DescriptorIndex = static_cast<uint32_t>(m_BufferInfos.size());

				m_BufferInfos.emplace_back(vk::DescriptorBufferInfo()
					.setBuffer(nullptr)
					.setOffset(0u)
					.setRange(0u));

				Write.setPBufferInfo(&m_BufferInfos.back());
				break;
			case ERHIResourceType::SampledImage:
			case ERHIResourceType::StorageImage:
				ResourceInfo.DescriptorIndex = static_cast<uint32_t>(m_ImageInfos.size());

				m_ImageInfos.emplace_back(vk::DescriptorImageInfo()
					.setImageView(nullptr)
					.setImageLayout(vk::ImageLayout::eUndefined)
					.setSampler(nullptr));
				Write.setPImageInfo(&m_ImageInfos.back());
				break;
			case ERHIResourceType::CombinedImageSampler:
				ResourceInfo.DescriptorIndex = static_cast<uint32_t>(m_ImageInfos.size());

				m_ImageInfos.emplace_back(vk::DescriptorImageInfo()
					.setImageView(nullptr)
					.setImageLayout(vk::ImageLayout::eUndefined)
					.setSampler(nullptr));
				Write.setPImageInfo(&m_ImageInfos.back());
				break;
			case ERHIResourceType::Sampler:
				ResourceInfo.DescriptorIndex = static_cast<uint32_t>(m_ImageInfos.size());

				m_ImageInfos.emplace_back(vk::DescriptorImageInfo()
					.setImageView(nullptr)
					.setImageLayout(vk::ImageLayout::eUndefined)
					.setSampler(nullptr));
				Write.setPImageInfo(&m_ImageInfos.back());
				break;
			default:
				assert(false);
				break;
			}
		}
	}
}
