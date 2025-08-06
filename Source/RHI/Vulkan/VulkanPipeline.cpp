#include "RHI/Vulkan/VulkanPipeline.h"
#include "RHI/Vulkan/VulkanDevice.h"
#include "RHI/Vulkan/VulkanRHI.h"
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
	for (auto& Shader : Desc.ShaderPipeline)
	{
		if (Shader)
		{
			auto ShaderModule = Cast<VulkanShader>(ShaderLibrary::Get().GetShaderModule(*Shader));
			assert(ShaderModule);

			ShaderStageCreateInfos.emplace_back(vk::PipelineShaderStageCreateInfo())
				.setStage(GetShaderStage(Shader->GetStage()))
				.setModule(ShaderModule->GetNative())
				.setPName("main");
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
	
	vk::PipelineDynamicStateCreateInfo DynamicStateCreateInfo;
	if (VulkanRHI::GetExtConfigs().DynamicState)
	{
		std::vector<vk::DynamicState> DynamicStates
		{
			vk::DynamicState::eViewport,
			vk::DynamicState::eScissor,
			//vk::DynamicState::eCullMode,
			//vk::DynamicState::ePolygonModeEXT,
			//vk::DynamicState::eFrontFace,
			//vk::DynamicState::ePrimitiveTopology,
		};
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
		.setLayout(Cast<VulkanPipelineState>(m_PipelineState)->GetPipelineLayout())
		.setRenderPass(nullptr);

	/// #TODO Batch creations ???
	VERIFY_VK(GetNativeDevice().createGraphicsPipelines(PipelineCache, 1u, &PipelineCreateInfo, VK_ALLOCATION_CALLBACKS, &m_Native));
}

VulkanPipelineState::VulkanPipelineState(const VulkanDevice& Device, const RHIGraphicsPipelineDesc& Desc)
	: VkBaseDeviceResource(Device)
	, RHIPipelineState(Desc)
{
	m_DescriptorSetLayout = std::make_unique<VulkanDescriptorSetLayout>(Device, GetShaderResourceLayout());
	m_PipelineLayout = std::make_unique<VulkanPipelineLayout>(Device, m_DescriptorSetLayout->GetNative());
}

void VulkanPipelineState::CommitShaderResources(RHICommandBuffer* CommandBuffer)
{
	assert(CommandBuffer);

	if (!m_Set)
	{
		assert(false);
		//auto Context = Cast<VulkanCommandListContext>(CommandBuffer->GetContext());
		//m_Set = Context->AcquireDescriptorPool().Allocate(m_DescriptorSetLayout->GetNative());
		//InitWrites();
	}

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
				m_ImageInfos[ResourceInfo.DescriptorIndex].setImageView(nullptr)
					.setImageLayout(vk::ImageLayout::eUndefined);
				break;
			case ERHIResourceType::CombinedImageSampler:
				m_ImageInfos[ResourceInfo.DescriptorIndex].setImageView(nullptr)
					.setSampler(nullptr)
					.setImageLayout(vk::ImageLayout::eUndefined);
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

void VulkanPipelineState::CommitPipelineStates(RHICommandBuffer* CommandBuffer)
{
	assert(CommandBuffer);
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

				m_BufferInfos.emplace_back(vk::DescriptorBufferInfo())
					.setBuffer(nullptr)
					.setOffset(0u)
					.setRange(0u);

				Write.setPBufferInfo(&m_BufferInfos.back());
				break;
			case ERHIResourceType::SampledImage:
			case ERHIResourceType::StorageImage:
				ResourceInfo.DescriptorIndex = static_cast<uint32_t>(m_ImageInfos.size());

				m_ImageInfos.emplace_back(vk::DescriptorImageInfo())
					.setImageView(nullptr)
					.setImageLayout(vk::ImageLayout::eUndefined)
					.setSampler(nullptr);
				Write.setPImageInfo(&m_ImageInfos.back());
				break;
			case ERHIResourceType::CombinedImageSampler:
				ResourceInfo.DescriptorIndex = static_cast<uint32_t>(m_ImageInfos.size());

				m_ImageInfos.emplace_back(vk::DescriptorImageInfo())
					.setImageView(nullptr)
					.setImageLayout(vk::ImageLayout::eUndefined)
					.setSampler(nullptr);
				Write.setPImageInfo(&m_ImageInfos.back());
				break;
			case ERHIResourceType::Sampler:
				ResourceInfo.DescriptorIndex = static_cast<uint32_t>(m_ImageInfos.size());

				m_ImageInfos.emplace_back(vk::DescriptorImageInfo())
					.setImageView(nullptr)
					.setImageLayout(vk::ImageLayout::eUndefined)
					.setSampler(nullptr);
				Write.setPImageInfo(&m_ImageInfos.back());
				break;
			default:
				assert(false);
				break;
			}
		}
	}
}
