#include "RHI/D3D/D3D12/D3D12Pipeline.h"
#include "RHI/D3D/D3D12/D3D12Device.h"
#include "RHI/D3D/D3D12/D3D12Shader.h"
#include "Engine/Services/SpdLogService.h"

D3D12RootSignature::D3D12RootSignature(const D3D12Device& Device, const RHIGraphicsPipelineCreateInfo& RHICreateInfo)
{
	/// https://docs.microsoft.com/en-us/windows/win32/api/d3d12/ne-d3d12-d3d12_descriptor_range_flags
	std::vector<D3D12_ROOT_PARAMETER1> RootParams;
	std::vector<D3D12_DESCRIPTOR_RANGE1> DescriptorRangesSamplers;
	std::vector<D3D12_DESCRIPTOR_RANGE1> DescriptorRangesSRVs;
	D3D12_ROOT_CONSTANTS RootConstants{};

#if 0
	size_t NumSamplers = 0u;
	size_t NumSRVs = 0u;
	RHICreateInfo.Shaders.ForEach([
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
#endif

	Create(Device, RootParams, RHICreateInfo);
}

void D3D12RootSignature::Create(const D3D12Device& Device, const std::vector<D3D12_ROOT_PARAMETER1>& Params, const RHIGraphicsPipelineCreateInfo& RHICreateInfo)
{
	D3DBlob Signature;
	D3DBlob Error;

	D3D12_VERSIONED_ROOT_SIGNATURE_DESC CreateDesc
	{
		//Device->Features().RootSignatureVersion
	};

	if (CreateDesc.Version == D3D_ROOT_SIGNATURE_VERSION_1_1)
	{
		CreateDesc.Desc_1_1.NumParameters = static_cast<uint32_t>(Params.size());
		CreateDesc.Desc_1_1.pParameters = Params.data();
		CreateDesc.Desc_1_1.NumStaticSamplers = 0u;
		CreateDesc.Desc_1_1.pStaticSamplers = nullptr;
		CreateDesc.Desc_1_1.Flags = D3D12_ROOT_SIGNATURE_FLAG_NONE;
		//if (RHICreateInfo.InputLayout)
		//{
		//	CreateDesc.Desc_1_1.Flags |= D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
		//}

		if (FAILED(D3D12SerializeVersionedRootSignature(&CreateDesc, Signature.Reference(), Error.Reference())))
		{
			LOG_ERROR("Failed to serialize versioned root signature: {}", reinterpret_cast<const char*>(Error->GetBufferPointer()));
			assert(false);
		}
	}
	else
	{
		assert(false);
	}

	VERIFY_D3D(Device->CreateRootSignature(0u, Signature->GetBufferPointer(), Signature->GetBufferSize(), IID_PPV_ARGS(Reference())));
}

D3D12GraphicsPipelineState::D3D12GraphicsPipelineState(const D3D12Device& Device, const RHIGraphicsPipelineCreateInfo& RHICreateInfo)
{
	m_RootSignature = std::make_unique<D3D12RootSignature>(Device, RHICreateInfo);

	D3D12_SHADER_BYTECODE ShaderByteCodes[(size_t)ERHIShaderStage::Num]{};
	for (uint32_t Stage = (size_t)ERHIShaderStage::Vertex; Stage < (size_t)ERHIShaderStage::Compute; ++Stage)
	{
		//if (auto Shader = RHICreateInfo.Shaders.Get(static_cast<ERHIShaderStage>(Stage)))
		//{
		//	Shaders[Stage].BytecodeLength = Shader->Desc()->BinarySize;
		//	Shaders[Stage].pShaderBytecode = Shader->Desc()->Binary.data();
		//}
	}

	D3D12_GRAPHICS_PIPELINE_STATE_DESC CreateDesc
	{
		m_RootSignature->GetNative(),
		ShaderByteCodes[(size_t)ERHIShaderStage::Vertex],
		ShaderByteCodes[(size_t)ERHIShaderStage::Fragment],
		ShaderByteCodes[(size_t)ERHIShaderStage::Domain],
		ShaderByteCodes[(size_t)ERHIShaderStage::Hull],
		ShaderByteCodes[(size_t)ERHIShaderStage::Geometry],
		D3D12_STREAM_OUTPUT_DESC{}
	};

	/**********************************************************************************************
		IndependentBlendEnable:
		Specifies whether to enable independent blending in simultaneous render targets. 
		Set to TRUE to enable independent blending. 
		If set to FALSE, only the RenderTarget[0] members are used; RenderTarget[1..7] are ignored.
	***********************************************************************************************/
	CreateDesc.BlendState.AlphaToCoverageEnable = RHICreateInfo.MultisampleState.EnableAlphaToCoverage;
	CreateDesc.BlendState.IndependentBlendEnable = true;
	for (uint32_t Index = 0u; Index < ERHILimitations::MaxRenderTargets; ++Index)
	{
		if (RHICreateInfo.BlendState.RenderTargetBlends[Index].Enable)
		{
			CreateDesc.BlendState.RenderTarget[Index] = D3D12_RENDER_TARGET_BLEND_DESC
			{
				.BlendEnable = RHICreateInfo.BlendState.RenderTargetBlends[Index].Enable,
				.LogicOpEnable = RHICreateInfo.BlendState.EnableLogicOp,
				.SrcBlend = GetBlendFactor(RHICreateInfo.BlendState.RenderTargetBlends[Index].SrcColor),
				.DestBlend = GetBlendFactor(RHICreateInfo.BlendState.RenderTargetBlends[Index].DstColor),
				.BlendOp = GetBlendOp(RHICreateInfo.BlendState.RenderTargetBlends[Index].ColorOp),
				.SrcBlendAlpha = GetBlendFactor(RHICreateInfo.BlendState.RenderTargetBlends[Index].SrcAlpha),
				.DestBlendAlpha = GetBlendFactor(RHICreateInfo.BlendState.RenderTargetBlends[Index].DstAlpha),
				.BlendOpAlpha = GetBlendOp(RHICreateInfo.BlendState.RenderTargetBlends[Index].AlphaOp),
				.LogicOp = GetLogicOp(RHICreateInfo.BlendState.LogicOp),
				.RenderTargetWriteMask = GetColorComponentMask(RHICreateInfo.BlendState.RenderTargetBlends[Index].ColorMask)
			};
		}
	}

	CreateDesc.SampleMask = *RHICreateInfo.MultisampleState.SampleMask; /// #TODO

	CreateDesc.RasterizerState = D3D12_RASTERIZER_DESC
	{
		.FillMode = GetPolygonMode(RHICreateInfo.RasterizationState.PolygonMode),
		.CullMode = GetCullMode(RHICreateInfo.RasterizationState.CullMode),
		.FrontCounterClockwise = RHICreateInfo.RasterizationState.FrontFace == ERHIFrontFace::Counterclockwise,
		.DepthBias = static_cast<int32_t>(RHICreateInfo.RasterizationState.DepthBias),
		.DepthBiasClamp = RHICreateInfo.RasterizationState.DepthBiasClamp,
		.SlopeScaledDepthBias = RHICreateInfo.RasterizationState.DepthBiasSlope,
		.DepthClipEnable = RHICreateInfo.RasterizationState.EnableDepthClamp,
		.MultisampleEnable = RHICreateInfo.MultisampleState.SampleCount > ERHISampleCount::Sample_1_Bit,
		.AntialiasedLineEnable = false,
		.ForcedSampleCount = static_cast<uint32_t>(RHICreateInfo.MultisampleState.SampleCount),
		.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF
	};

	D3D12_DEPTH_STENCILOP_DESC FrontFace
	{
		.StencilFailOp = GetStencilOp(RHICreateInfo.DepthStencilState.FrontFaceStencil.FailOp),
		.StencilDepthFailOp = GetStencilOp(RHICreateInfo.DepthStencilState.FrontFaceStencil.DepthFailOp),
		.StencilPassOp = GetStencilOp(RHICreateInfo.DepthStencilState.FrontFaceStencil.PassOp),
		.StencilFunc = GetCompareFunc(RHICreateInfo.DepthStencilState.FrontFaceStencil.CompareFunc)
	};
	D3D12_DEPTH_STENCILOP_DESC BackFace
	{
		.StencilFailOp = GetStencilOp(RHICreateInfo.DepthStencilState.BackFaceStencil.FailOp),
		.StencilDepthFailOp = GetStencilOp(RHICreateInfo.DepthStencilState.BackFaceStencil.DepthFailOp),
		.StencilPassOp = GetStencilOp(RHICreateInfo.DepthStencilState.BackFaceStencil.PassOp),
		.StencilFunc = GetCompareFunc(RHICreateInfo.DepthStencilState.BackFaceStencil.CompareFunc)
	};

	CreateDesc.DepthStencilState = D3D12_DEPTH_STENCIL_DESC
	{
		.DepthEnable = RHICreateInfo.DepthStencilState.EnableDepth,
		.DepthWriteMask = RHICreateInfo.DepthStencilState.EnableDepth ? D3D12_DEPTH_WRITE_MASK_ALL : D3D12_DEPTH_WRITE_MASK_ZERO,
		.DepthFunc = GetCompareFunc(RHICreateInfo.DepthStencilState.DepthCompareFunc),
		.StencilEnable = RHICreateInfo.DepthStencilState.EnableStencil,
		.StencilReadMask = RHICreateInfo.DepthStencilState.StencilReadMask,
		.StencilWriteMask = RHICreateInfo.DepthStencilState.StencilWriteMask,
		.FrontFace = FrontFace,
		.BackFace = BackFace
	};

	//const auto& InputElements = (Cast<D3D12InputLayout>(RHICreateInfo.InputLayout))->InputElementDescs();
	//CreateDesc.InputLayout = D3D12_INPUT_LAYOUT_DESC
	//{
	//	InputElements.data(),
	//	static_cast<uint32_t>(InputElements.size())
	//};

	CreateDesc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;

	CreateDesc.PrimitiveTopologyType = GetPrimitiveTopology(RHICreateInfo.PrimitiveTopology);

#if 0
	const RHIFrameBufferCreateInfo& FrameBufferDesc = RHICreateInfo.FrameBuffer->Description();
	CreateDesc.NumRenderTargets = static_cast<uint32_t>(FBDesc.ColorAttachments.size());
	for (uint32_t i = 0u; i < FBDesc.ColorAttachments.size(); ++i)
	{
		CreateDesc.RTVFormats[i] = D3D12Type::Format(FBDesc.ColorAttachments[i]->Format());
	}

	if (FBDesc.DepthStencilAttachment)
	{
		CreateDesc.DSVFormat = D3D12Type::Format(FBDesc.DepthStencilAttachment->Format());
	}
#endif

	CreateDesc.SampleDesc = DXGI_SAMPLE_DESC
	{
		static_cast<uint32_t>(RHICreateInfo.MultisampleState.SampleCount),
		0u
	};

	CreateDesc.NodeMask = 0u;

	CreateDesc.CachedPSO = D3D12_CACHED_PIPELINE_STATE{};

	CreateDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

	VERIFY_D3D(Device->CreateGraphicsPipelineState(&CreateDesc, IID_PPV_ARGS(Reference())));
}

D3D12GraphicsPipeline::D3D12GraphicsPipeline(const D3D12Device& Device, const RHIGraphicsPipelineCreateInfo& RHICreateInfo)
{
	//m_State = std::make_shared<D3D12GraphicsPipelineState>(Device, RHICreateInfo);
}

#if 0
void D3D12GraphicsPipelineState::WriteImage(EShaderStage Stage, uint32_t Binding, IImage* Image)
{
}

void D3D12GraphicsPipelineState::WriteSampler(EShaderStage Stage, uint32_t Binding, ISampler* Sampler)
{
}

void D3D12GraphicsPipelineState::WriteUniformBuffer(EShaderStage Stage, uint32_t Binding, IBuffer* Buffer)
{
}
#endif
