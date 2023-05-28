#include "Colorful/D3D/D3D12/D3D12Pipeline.h"
#include "Colorful/D3D/D3D12/D3D12Device.h"

NAMESPACE_START(RHI)

D3D12RootSignature::D3D12RootSignature(D3D12Device* Device, const GraphicsPipelineDesc& Desc)
{
	assert(Device);
	/// https://docs.microsoft.com/en-us/windows/win32/api/d3d12/ne-d3d12-d3d12_descriptor_range_flags
	std::vector<D3D12_ROOT_PARAMETER1> RootParams;
	std::vector<D3D12_DESCRIPTOR_RANGE1> DescriptorRangesSamplers;
	std::vector<D3D12_DESCRIPTOR_RANGE1> DescriptorRangesSRVs;
	D3D12_ROOT_CONSTANTS RootConstants{};

	size_t NumSamplers = 0u;
	size_t NumSRVs = 0u;
	Desc.Shaders.ForEach([
		this, 
		&RootParams, 
		&DescriptorRangesSamplers, 
		&DescriptorRangesSRVs, 
		&RootConstants,
		&NumSamplers,
		&NumSRVs](const IShader* Shader) 
	{
		for (auto& Variable : Shader->Desc()->Variables)
		{
			switch (Variable.Type)
			{
			case EResourceType::Sampler:
				DescriptorRangesSamplers.emplace_back(
					D3D12_DESCRIPTOR_RANGE1
					{
						D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER,
						1u,
						Variable.Binding,
						0u,  /// #TODO
						D3D12_DESCRIPTOR_RANGE_FLAG_NONE,
						D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND
					});
				break;
			case EResourceType::SampledImage:
			case EResourceType::UniformTexelBuffer:
				DescriptorRangesSRVs.emplace_back(
					D3D12_DESCRIPTOR_RANGE1
					{
						D3D12_DESCRIPTOR_RANGE_TYPE_SRV,
						1u,
						Variable.Binding,
						0u,  /// #TODO
						D3D12_DESCRIPTOR_RANGE_FLAG_DATA_VOLATILE,
						D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND
					});
				break;
			case EResourceType::StorageImage:
			case EResourceType::StorageTexelBuffer:
			case EResourceType::StorageBuffer:
			case EResourceType::StorageBufferDynamic:
				DescriptorRangesSRVs.emplace_back(
					D3D12_DESCRIPTOR_RANGE1
					{
						D3D12_DESCRIPTOR_RANGE_TYPE_UAV,
						1u,
						Variable.Binding,
						0u,  /// #TODO
						D3D12_DESCRIPTOR_RANGE_FLAG_DATA_VOLATILE,
						D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND
					});
				break;
			case EResourceType::UniformBuffer:
			case EResourceType::UniformBufferDynamic:
				DescriptorRangesSRVs.emplace_back(
					D3D12_DESCRIPTOR_RANGE1
					{
						D3D12_DESCRIPTOR_RANGE_TYPE_CBV,
						1u,
						Variable.Binding,
						0u,  /// #TODO
						D3D12_DESCRIPTOR_RANGE_FLAG_DATA_VOLATILE,
						D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND
					});
				break;
			case EResourceType::PushConstants:
				RootConstants.ShaderRegister = Variable.Binding;
				RootConstants.RegisterSpace = 0u; /// #TODO
				RootConstants.Num32BitValues = Variable.ConstantsSize / sizeof(uint32_t);
				break;
			}
		}

		if (DescriptorRangesSamplers.size() > NumSamplers)
		{
			RootParams.emplace_back(
				D3D12_ROOT_PARAMETER1
				{
					D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE,
					D3D12_ROOT_DESCRIPTOR_TABLE1
					{
						static_cast<uint32_t>(DescriptorRangesSamplers.size() - NumSamplers),
						DescriptorRangesSamplers.data() + NumSamplers
					},
					D3D12Type::ShaderVisibility(Shader->Stage())
				});
			NumSamplers += (DescriptorRangesSamplers.size() - NumSamplers);
		}
		if (DescriptorRangesSRVs.size() > NumSRVs)
		{
			RootParams.emplace_back(
				D3D12_ROOT_PARAMETER1
				{
					D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE,
					D3D12_ROOT_DESCRIPTOR_TABLE1
					{
						static_cast<uint32_t>(DescriptorRangesSRVs.size() - NumSRVs),
						DescriptorRangesSRVs.data() + NumSRVs
					},
					D3D12Type::ShaderVisibility(Shader->Stage())
				});
			NumSRVs += (DescriptorRangesSRVs.size() - NumSRVs);
		}
		if (RootConstants.Num32BitValues > 0u)
		{
			D3D12_ROOT_PARAMETER1 Param1;
			Param1.ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
			Param1.Constants = RootConstants;
			Param1.ShaderVisibility = D3D12Type::ShaderVisibility(Shader->Stage());
			RootParams.emplace_back(std::move(Param1));
		}
	});

	Create(Device, RootParams, Desc);
}

void D3D12RootSignature::Create(D3D12Device* Device, const std::vector<D3D12_ROOT_PARAMETER1>& Params, const GraphicsPipelineDesc& Desc)
{
	D3DBlob Signature;
	D3DBlob Error;

	D3D12_VERSIONED_ROOT_SIGNATURE_DESC CreateDesc
	{
		Device->Features().RootSignatureVersion
	};

	if (CreateDesc.Version == D3D_ROOT_SIGNATURE_VERSION_1_1)
	{
		CreateDesc.Desc_1_1.NumParameters = static_cast<uint32_t>(Params.size());
		CreateDesc.Desc_1_1.pParameters = Params.data();
		CreateDesc.Desc_1_1.NumStaticSamplers = 0u;
		CreateDesc.Desc_1_1.pStaticSamplers = nullptr;
		CreateDesc.Desc_1_1.Flags = D3D12_ROOT_SIGNATURE_FLAG_NONE;
		if (Desc.InputLayout)
		{
			CreateDesc.Desc_1_1.Flags |= D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
		}

		if (FAILED(D3D12SerializeVersionedRootSignature(&CreateDesc, Signature.Reference(), Error.Reference())))
		{
			LOG_ERROR("Failed to serialize versioned root signature: {}", reinterpret_cast<const char8_t*>(Error.Get()->GetBufferPointer()));
			assert(false);
		}
	}
	else
	{
		assert(false);
	}

	VERIFY_D3D(Device->Get()->CreateRootSignature(0u, Signature->GetBufferPointer(), Signature->GetBufferSize(), IID_PPV_ARGS(Reference())));
}

D3D12GraphicsPipelineState::D3D12GraphicsPipelineState(D3D12Device* Device, const GraphicsPipelineDesc& Desc)
	: D3DHWObject(Desc)
{
	assert(Device);

	m_RootSignature = std::make_unique<D3D12RootSignature>(Device, Desc);

	D3D12_SHADER_BYTECODE Shaders[EShaderStage::Compute]{};
	for (uint32_t Stage = EShaderStage::Vertex; Stage < EShaderStage::Compute; ++Stage)
	{
		if (auto Shader = Desc.Shaders.Get(static_cast<EShaderStage>(Stage)))
		{
			Shaders[Stage].BytecodeLength = Shader->Desc()->BinarySize;
			Shaders[Stage].pShaderBytecode = Shader->Desc()->Binary.data();
		}
	}

	D3D12_GRAPHICS_PIPELINE_STATE_DESC CreateDesc
	{
		m_RootSignature->Get(),
		Shaders[EShaderStage::Vertex],
		Shaders[EShaderStage::Fragment],
		Shaders[EShaderStage::Domain],
		Shaders[EShaderStage::Hull],
		Shaders[EShaderStage::Geometry],
		D3D12_STREAM_OUTPUT_DESC{}
	};

	/**********************************************************************************************
		IndependentBlendEnable:
		Specifies whether to enable independent blending in simultaneous render targets. 
		Set to TRUE to enable independent blending. 
		If set to FALSE, only the RenderTarget[0] members are used; RenderTarget[1..7] are ignored.
	***********************************************************************************************/
	CreateDesc.BlendState.AlphaToCoverageEnable = Desc.MultisampleState.EnableAlphaToCoverage;
	CreateDesc.BlendState.IndependentBlendEnable = true;
	for (uint32_t Index = 0u; Index < ELimitations::MaxRenderTargets; ++Index)
	{
		if (Desc.BlendState.RenderTargetBlends[Index].Index == Index)
		{
			CreateDesc.BlendState.RenderTarget[Index] = D3D12_RENDER_TARGET_BLEND_DESC
			{
				Desc.BlendState.RenderTargetBlends[Index].Enable,
				Desc.BlendState.EnableLogicOp,
				D3D12Type::BlendFactor(Desc.BlendState.RenderTargetBlends[Index].SrcColor),
				D3D12Type::BlendFactor(Desc.BlendState.RenderTargetBlends[Index].DstColor),
				D3D12Type::BlendOp(Desc.BlendState.RenderTargetBlends[Index].ColorOp),
				D3D12Type::BlendFactor(Desc.BlendState.RenderTargetBlends[Index].SrcAlpha),
				D3D12Type::BlendFactor(Desc.BlendState.RenderTargetBlends[Index].DstAlpha),
				D3D12Type::BlendOp(Desc.BlendState.RenderTargetBlends[Index].AlphaOp),
				D3D12Type::LogicOp(Desc.BlendState.LogicOp),
				D3D12Type::ColorComponentMask(Desc.BlendState.RenderTargetBlends[Index].ColorMask)
			};
		}
	}

	CreateDesc.SampleMask = *Desc.MultisampleState.SampleMask; /// #TODO

	CreateDesc.RasterizerState = D3D12_RASTERIZER_DESC
	{
		D3D12Type::PolygonMode(Desc.RasterizationState.PolygonMode),
		D3D12Type::CullMode(Desc.RasterizationState.CullMode),
		Desc.RasterizationState.FrontFace == EFrontFace::Counterclockwise,
		static_cast<int32_t>(Desc.RasterizationState.DepthBias),
		Desc.RasterizationState.DepthBiasClamp,
		Desc.RasterizationState.DepthBiasSlope,
		Desc.RasterizationState.EnableDepthClamp,
		Desc.MultisampleState.SampleCount > ESampleCount::Sample_1_Bit,
		false,
		static_cast<uint32_t>(Desc.MultisampleState.SampleCount),
		D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF
	};

	CreateDesc.DepthStencilState = D3D12_DEPTH_STENCIL_DESC
	{
		Desc.DepthStencilState.EnableDepth,
		Desc.DepthStencilState.EnableDepth ? D3D12_DEPTH_WRITE_MASK_ALL : D3D12_DEPTH_WRITE_MASK_ZERO,
		D3D12Type::CompareFunc(Desc.DepthStencilState.DepthCompareFunc),
		Desc.DepthStencilState.EnableStencil,
		Desc.DepthStencilState.StencilReadMask,
		Desc.DepthStencilState.StencilWriteMask,
		D3D12_DEPTH_STENCILOP_DESC
		{
			D3D12Type::StencilOp(Desc.DepthStencilState.FrontFaceStencil.FailOp),
			D3D12Type::StencilOp(Desc.DepthStencilState.FrontFaceStencil.DepthFailOp),
			D3D12Type::StencilOp(Desc.DepthStencilState.FrontFaceStencil.PassOp),
			D3D12Type::CompareFunc(Desc.DepthStencilState.FrontFaceStencil.CompareFunc)
		},
		D3D12_DEPTH_STENCILOP_DESC
		{
			D3D12Type::StencilOp(Desc.DepthStencilState.BackFaceStencil.FailOp),
			D3D12Type::StencilOp(Desc.DepthStencilState.BackFaceStencil.DepthFailOp),
			D3D12Type::StencilOp(Desc.DepthStencilState.BackFaceStencil.PassOp),
			D3D12Type::CompareFunc(Desc.DepthStencilState.BackFaceStencil.CompareFunc)
		}
	};

	const auto& InputElements = (static_cast<D3D12InputLayout*>(Desc.InputLayout))->InputElementDescs();
	CreateDesc.InputLayout = D3D12_INPUT_LAYOUT_DESC
	{
		InputElements.data(),
		static_cast<uint32_t>(InputElements.size())
	};

	CreateDesc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;

	CreateDesc.PrimitiveTopologyType = D3D12Type::PrimitiveTopology(Desc.PrimitiveTopology);

	const FrameBufferDesc& FBDesc = Desc.FrameBuffer->Description();
	CreateDesc.NumRenderTargets = static_cast<uint32_t>(FBDesc.ColorAttachments.size());
	for (uint32_t i = 0u; i < FBDesc.ColorAttachments.size(); ++i)
	{
		CreateDesc.RTVFormats[i] = D3D12Type::Format(FBDesc.ColorAttachments[i]->Format());
	}

	if (FBDesc.DepthStencilAttachment)
	{
		CreateDesc.DSVFormat = D3D12Type::Format(FBDesc.DepthStencilAttachment->Format());
	}

	CreateDesc.SampleDesc = DXGI_SAMPLE_DESC
	{
		static_cast<uint32_t>(Desc.MultisampleState.SampleCount),
		0u
	};

	CreateDesc.NodeMask = 0u;

	CreateDesc.CachedPSO = D3D12_CACHED_PIPELINE_STATE{};

	CreateDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

	VERIFY_D3D(Device->Get()->CreateGraphicsPipelineState(&CreateDesc, IID_PPV_ARGS(Reference())));
}

D3D12GraphicsPipeline::D3D12GraphicsPipeline(D3D12Device* Device, const GraphicsPipelineDesc& Desc)
{
	m_State = std::make_shared<D3D12GraphicsPipelineState>(Device, Desc);
}

void D3D12GraphicsPipelineState::WriteImage(EShaderStage Stage, uint32_t Binding, IImage* Image)
{
}

void D3D12GraphicsPipelineState::WriteSampler(EShaderStage Stage, uint32_t Binding, ISampler* Sampler)
{
}

void D3D12GraphicsPipelineState::WriteUniformBuffer(EShaderStage Stage, uint32_t Binding, IBuffer* Buffer)
{
}

NAMESPACE_END(RHI)
