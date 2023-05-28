#if 0
VulkanRenderPassPtr VkRenderStateCache::getOrCreateRenderPass(const RenderPassDesc& Desc)
{
	for (auto& pair : m_RenderPassCache)
	{
		if (pair.first == Desc.hash())
		{
			return pair.second;
		}
	}

	if (Desc.AttachmentDescs.size() == 0u)
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
		m_RenderPassCache.push_back(std::make_pair(Desc.hash(), renderPass));
		return renderPass;
	}

	auto renderPass = create_shared<VulkanRenderPass>(Desc);
	m_RenderPassCache.push_back(std::make_pair(Desc.hash(), renderPass));
	return renderPass;
}
#else
#include "Colorful/Vulkan/VulkanPipeline.h"
#include "Colorful/Vulkan/VulkanDevice.h"

NAMESPACE_START(RHI)

VulkanPipelineCache::VulkanPipelineCache(VulkanDevice* Device)
	: VkHWObject(Device)
{
	VkPipelineCacheCreateInfo CreateInfo
	{
		VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO,
		nullptr,
		0u,   /// flags is reserved for future use
		0u,   /// #TODO "pInitialData"
		nullptr
	};
	VERIFY_VK(vkCreatePipelineCache(m_Device->Get(), &CreateInfo, VK_ALLOCATION_CALLBACKS, Reference()));
}

VulkanPipeline::VulkanPipeline(VulkanDevice* Device)
	: VkHWObject(Device)
{
}

VulkanGraphicsPipeline::VulkanGraphicsPipeline(VulkanDevice* Device, VkPipelineCache PipelineCache, const GraphicsPipelineDesc& Desc)
	: VulkanPipeline(Device)
{
	assert(Desc.FrameBuffer);

	std::vector<VkPipelineShaderStageCreateInfo> ShaderStageCreateInfos;
	Desc.Shaders.ForEach([&ShaderStageCreateInfos](IShader* Shader) {
		ShaderStageCreateInfos.emplace_back(
			VkPipelineShaderStageCreateInfo
			{
				VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
				nullptr,
				0u,
				VkType::ShaderStage(Shader->Stage()),
				static_cast<VulkanShader*>(Shader)->Get(),
				"main", /// #TODO
				nullptr /// pSpecializationInfo
			}
		);
	});
	assert(ShaderStageCreateInfos.size() > 0u);

	VkPipelineRasterizationStateCreateInfo RasterizationStateCreateInfo
	{
		VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
		nullptr,
		0u, /// flags is reserved for future use
		Desc.RasterizationState.EnableDepthClamp,
		false,
		VkType::PolygonMode(Desc.RasterizationState.PolygonMode),
		VkType::CullMode(Desc.RasterizationState.CullMode),
		VkType::FrontFace(Desc.RasterizationState.FrontFace),
		Desc.RasterizationState.DepthBias != 0.0f,
		Desc.RasterizationState.DepthBias,
		Desc.RasterizationState.DepthBiasClamp,
		Desc.RasterizationState.DepthBiasSlope,
		Desc.RasterizationState.LineWidth
	};

	VkPipelineDepthStencilStateCreateInfo DepthStencilStateCreateInfo
	{
		VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
		nullptr,
		0u,  /// flags is reserved for future use.
		Desc.DepthStencilState.EnableDepth,
		Desc.DepthStencilState.EnableDepthWrite,
		VkType::CompareFunc(Desc.DepthStencilState.DepthCompareFunc),
		Desc.DepthStencilState.EnableDepthBoundsTest,
		Desc.DepthStencilState.EnableStencil,
		{
			VkType::StencilOp(Desc.DepthStencilState.FrontFaceStencil.FailOp),
			VkType::StencilOp(Desc.DepthStencilState.FrontFaceStencil.PassOp),
			VkType::StencilOp(Desc.DepthStencilState.FrontFaceStencil.DepthFailOp),
			VkType::CompareFunc(Desc.DepthStencilState.FrontFaceStencil.CompareFunc),
			Desc.DepthStencilState.StencilReadMask,
			Desc.DepthStencilState.StencilWriteMask,
			Desc.DepthStencilState.FrontFaceStencil.Ref
		},
		{
			VkType::StencilOp(Desc.DepthStencilState.BackFaceStencil.FailOp),
			VkType::StencilOp(Desc.DepthStencilState.BackFaceStencil.PassOp),
			VkType::StencilOp(Desc.DepthStencilState.BackFaceStencil.DepthFailOp),
			VkType::CompareFunc(Desc.DepthStencilState.BackFaceStencil.CompareFunc),
			Desc.DepthStencilState.StencilReadMask,
			Desc.DepthStencilState.StencilWriteMask,
			Desc.DepthStencilState.BackFaceStencil.Ref
		},
		Desc.DepthStencilState.DepthBounds.x,
		Desc.DepthStencilState.DepthBounds.y
	};

	std::vector<VkPipelineColorBlendAttachmentState> ColorBlendAttachmentStates;
	for (uint32_t AttachmentIndex = 0u; AttachmentIndex < ELimitations::MaxRenderTargets; ++AttachmentIndex)
	{
		if (Desc.BlendState.RenderTargetBlends[AttachmentIndex].Index == AttachmentIndex)
		{
			ColorBlendAttachmentStates.emplace_back(
				VkPipelineColorBlendAttachmentState
				{
					Desc.BlendState.RenderTargetBlends[AttachmentIndex].Enable,
					VkType::BlendFactor(Desc.BlendState.RenderTargetBlends[AttachmentIndex].SrcColor),
					VkType::BlendFactor(Desc.BlendState.RenderTargetBlends[AttachmentIndex].DstColor),
					VkType::BlendOp(Desc.BlendState.RenderTargetBlends[AttachmentIndex].ColorOp),
					VkType::BlendFactor(Desc.BlendState.RenderTargetBlends[AttachmentIndex].SrcAlpha),
					VkType::BlendFactor(Desc.BlendState.RenderTargetBlends[AttachmentIndex].DstAlpha),
					VkType::BlendOp(Desc.BlendState.RenderTargetBlends[AttachmentIndex].AlphaOp),
					VkType::ColorComponentFlags(Desc.BlendState.RenderTargetBlends[AttachmentIndex].ColorMask)
				}
			);
		}
	}
	VkPipelineColorBlendStateCreateInfo BlendStateCreateInfo
	{
		VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
		nullptr,
		0u,  /// flags is reserved for future use.
		Desc.BlendState.EnableLogicOp,
		VkType::LogicOp(Desc.BlendState.LogicOp),
		static_cast<uint32_t>(ColorBlendAttachmentStates.size()),
		ColorBlendAttachmentStates.data()
	};

	VkPipelineTessellationStateCreateInfo TessellationStateCreateInfo
	{
		VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO
	};

	VkPipelineViewportStateCreateInfo ViewportStateCreateInfo
	{
		VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
		nullptr,
		0u,
		1u,
		nullptr,
		1u,
		nullptr
	};

	VkPipelineMultisampleStateCreateInfo MultisampleStateCreateInfo
	{
		VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
		nullptr,
		0u,
		static_cast<VkSampleCountFlagBits>(Desc.MultisampleState.SampleCount),
		Desc.MultisampleState.EnableSampleShading,
		Desc.MultisampleState.MinSampleShading,
		Desc.MultisampleState.SampleMask,
		Desc.MultisampleState.EnableAlphaToCoverage,
		Desc.MultisampleState.EnableAlphaToOne
	};

	/// Set viewport/scissor dynamic
	std::vector<VkDynamicState> DynamicStates
	{
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR
	};

#if VK_EXT_extended_dynamic_state
	/// Provided by VK_VERSION_1_3
	/// VK_DYNAMIC_STATE_CULL_MODE specifies that the cullMode state in VkPipelineRasterizationStateCreateInfo will be ignored 
	/// and must be set dynamically with vkCmdSetCullMode before any drawing commands.
	if (m_Device->EnabledExtensions().ExtendedDynamicState)
	{
		DynamicStates.push_back(VK_DYNAMIC_STATE_CULL_MODE_EXT);
		DynamicStates.push_back(VK_DYNAMIC_STATE_FRONT_FACE_EXT);
		DynamicStates.push_back(VK_DYNAMIC_STATE_PRIMITIVE_TOPOLOGY_EXT);
	}
#endif

	VkPipelineDynamicStateCreateInfo DynamicStateCreateInfo
	{
		VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
		nullptr,
		0u,
		static_cast<uint32_t>(DynamicStates.size()),
		DynamicStates.data()
	};

	VkPipelineVertexInputStateCreateInfo InputStateCreateInfo
	{
		VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO
	};

	if (auto InputLayout = static_cast<const VulkanInputLayout*>(Desc.InputLayout))
	{
		InputStateCreateInfo = InputLayout->InputStateCreateInfo();
	}
	
	VkPipelineInputAssemblyStateCreateInfo InputAssemblyStateCreateInfo
	{
		VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
		nullptr,
		0u,
		VkType::PrimitiveTopology(Desc.PrimitiveTopology),
		false /// #TODO primitiveRestartEnable
	};

	m_Descriptor = m_Device->GetOrAllocateDescriptor(Desc);

	VkGraphicsPipelineCreateInfo CreateInfo
	{
		VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
		nullptr,
		0u,
		static_cast<uint32_t>(ShaderStageCreateInfos.size()),
		ShaderStageCreateInfos.data(),
		&InputStateCreateInfo,
		&InputAssemblyStateCreateInfo,
		&TessellationStateCreateInfo,
		&ViewportStateCreateInfo,
		&RasterizationStateCreateInfo,
		&MultisampleStateCreateInfo,
		&DepthStencilStateCreateInfo,
		&BlendStateCreateInfo,
		&DynamicStateCreateInfo,
		m_Descriptor.PipelineLayout,
		static_cast<VulkanFramebuffer*>(Desc.FrameBuffer)->RenderPass(),
		0u,
		VK_NULL_HANDLE,
		0u
	};

	/// #TODO Batch creations ???
	VERIFY_VK(vkCreateGraphicsPipelines(m_Device->Get(), PipelineCache, 1u, &CreateInfo, VK_ALLOCATION_CALLBACKS, Reference()));

	m_State = std::make_shared<VulkanGraphicsPipelineState>(Desc, m_Descriptor.DescriptorSet);
}

VulkanPipelineCache::~VulkanPipelineCache()
{
	vkDestroyPipelineCache(m_Device->Get(), Get(), VK_ALLOCATION_CALLBACKS);
}

VulkanPipeline::~VulkanPipeline()
{
	vkDestroyDescriptorSetLayout(m_Device->Get(), m_Descriptor.DescriptorSetLayout, VK_ALLOCATION_CALLBACKS);

	vkDestroyPipelineLayout(m_Device->Get(), m_Descriptor.PipelineLayout, VK_ALLOCATION_CALLBACKS);

	vkDestroyPipeline(m_Device->Get(), Get(), VK_ALLOCATION_CALLBACKS);
}

VulkanGraphicsPipelineState::VulkanGraphicsPipelineState(const GraphicsPipelineDesc& Desc, VkDescriptorSet DescriptorSet)
	: PipelineState(Desc)
	, m_DescriptorSet(DescriptorSet)
{
	assert(m_DescriptorSet != VK_NULL_HANDLE);

	Desc.Shaders.ForEach([this, DescriptorSet](const IShader* Shader) {
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

NAMESPACE_END(RHI)
#endif
