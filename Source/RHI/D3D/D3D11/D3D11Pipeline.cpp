#include "Colorful/D3D/D3D11/D3D11Pipeline.h"
#include "Colorful/D3D/D3D11/D3D11EnumTranslator.h"

NAMESPACE_START(Gfx)

D3D11RasterizerState::D3D11RasterizerState(ID3D11Device* device, const RasterizationStateDesc& rasterizationStateDesc, const MultisampleStateDesc& multisampleStateDesc)
{
	assert(device);

	/// https://docs.microsoft.com/en-us/windows/win32/direct3d11/d3d10-graphics-programming-guide-output-merger-stage-depth-bias
	D3D11_RASTERIZER_DESC rasterizerDesc
	{
		D3D11EnumTranslator::polygonMode(rasterizationStateDesc.PolygonMode),
		D3D11EnumTranslator::cullMode(rasterizationStateDesc.CullMode),
		rasterizationStateDesc.FrontFace == EFrontFace::Counterclockwise ? true : false,
		rasterizationStateDesc.DepthBias > 0.0f ? static_cast<int32_t>(rasterizationStateDesc.DepthBias) : 0,
		rasterizationStateDesc.DepthBiasClamp,
		rasterizationStateDesc.DepthBiasSlope,
		rasterizationStateDesc.EnableDepthClamp,
		true, /// ScissorEnable
		multisampleStateDesc.SampleCount > ESampleCount::Sample_1_Bit,
		false
	};

	VERIFY_D3D(device->CreateRasterizerState(&rasterizerDesc, reference()));
}

D3D11BlendState::D3D11BlendState(ID3D11Device* device, const BlendStateDesc& blendStateDesc, const MultisampleStateDesc& multisampleStateDesc)
{
	assert(device);

	D3D11_BLEND_DESC blendDesc
	{
		multisampleStateDesc.EnableAlphaToCoverage,  /// AlphaToCoverageEnable
		false   /// Specifies whether to enable independent blending in simultaneous render targets. Set to TRUE to enable independent blending. If set to FALSE, only the RenderTarget[0] members are used; RenderTarget[1..7] are ignored.
	};

	for (uint32_t i = 0u; i < ELimitations::MaxRenderTargets; ++i)
	{
		blendDesc.RenderTarget[i] = D3D11_RENDER_TARGET_BLEND_DESC
		{
			blendStateDesc.RenderTargetBlends[i].Enable,
			D3D11EnumTranslator::blendFactor(blendStateDesc.RenderTargetBlends[i].SrcColor),
			D3D11EnumTranslator::blendFactor(blendStateDesc.RenderTargetBlends[i].DstColor),
			D3D11EnumTranslator::blendOp(blendStateDesc.RenderTargetBlends[i].ColorOp),
			D3D11EnumTranslator::blendFactor(blendStateDesc.RenderTargetBlends[i].SrcAlpha),
			D3D11EnumTranslator::blendFactor(blendStateDesc.RenderTargetBlends[i].DstColor),
			D3D11EnumTranslator::blendOp(blendStateDesc.RenderTargetBlends[i].AlphaOp),
			D3D11EnumTranslator::renderTargetWriteMask(blendStateDesc.RenderTargetBlends[i].ColorMask)
		};
	}

	VERIFY_D3D(device->CreateBlendState(&blendDesc, reference()));
}

D3D11DepthStencilState::D3D11DepthStencilState(ID3D11Device* device, const DepthStencilStateDesc& desc)
{
	assert(device);

	D3D11_DEPTH_STENCIL_DESC depthStencilDesc
	{
		desc.EnableDepth,
		desc.EnableDepthWrite ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO,
		D3D11EnumTranslator::compareFunc(desc.DepthCompareFunc),
		desc.EnableStencil,
		desc.StencilReadMask,
		desc.StencilWriteMask,
		D3D11_DEPTH_STENCILOP_DESC
		{
			D3D11EnumTranslator::stencilOp(desc.FrontFaceStencil.FailOp),
			D3D11EnumTranslator::stencilOp(desc.FrontFaceStencil.DepthFailOp),
			D3D11EnumTranslator::stencilOp(desc.FrontFaceStencil.PassOp),
			D3D11EnumTranslator::compareFunc(desc.FrontFaceStencil.CompareFunc),
		},
		D3D11_DEPTH_STENCILOP_DESC
		{
			D3D11EnumTranslator::stencilOp(desc.BackFaceStencil.FailOp),
			D3D11EnumTranslator::stencilOp(desc.BackFaceStencil.DepthFailOp),
			D3D11EnumTranslator::stencilOp(desc.BackFaceStencil.PassOp),
			D3D11EnumTranslator::compareFunc(desc.BackFaceStencil.CompareFunc),
		},
	};

	VERIFY_D3D(device->CreateDepthStencilState(&depthStencilDesc, reference()));
}

D3D11GraphicsPipeline::D3D11GraphicsPipeline(ID3D11Device* device, GraphicsPipelineDesc& desc)
{
	assert(device);

	///m_InputLayout = std::static_pointer_cast<D3D11InputLayout>(desc.material()->inputLayout());
	m_RasterizerState = std::make_unique<D3D11RasterizerState>(device, desc.RasterizationState, desc.MultisampleState);
	m_BlendState = std::make_unique<D3D11BlendState>(device, desc.BlendState, desc.MultisampleState);
	m_DepthStencilState = std::make_unique<D3D11DepthStencilState>(device, desc.DepthStencilState);
}

NAMESPACE_END(Gfx)

#if 0
#include "Colorful/D3D/D3D11/D3D11Engine.h"

void D3D11Context::setGraphicsPipeline(const D3D11GraphicsPipelinePtr& pipeline)
{
	assert(pipeline && pipeline->gfxPipelineState());
	auto gfxState = pipeline->gfxPipelineState();

#define ShaderCaster(Category)                                                                            \
{                                                                                                         \
	auto Category = static_cast<ID3D11##Category*>(std::static_pointer_cast<D3D11Shader>(shader)->get()); \
	if (Category != m_State.Category)                                                                     \
	{                                                                                                     \
		m_State.Category = Category;                                                                      \
		m_State.setDirty(D3D11PipelineState::eDirtyTag::e##Category, true);                               \
	}                                                                                                     \
}

	for (uint32_t i = 0u; i < eRShaderUsage_MaxEnum; ++i)
	{
		auto shader = gfxState->Material.Shaders[i];
		if (shader)
		{
			switch (i)
			{
			case eVertexShader:
				ShaderCaster(VertexShader)
				break;
			case eHullShader:
				ShaderCaster(HullShader)
				break;
			case eDomainShader:
				ShaderCaster(DomainShader)
				break;
			case eGeometryShader:
				ShaderCaster(GeometryShader)
				break;
			case eFragmentShader:
				ShaderCaster(PixelShader)
				break;
			case eComputeShader:
				ShaderCaster(ComputeShader)
				break;
			}
		}
	}

	setVertexIndexBuffers(pipeline);
	setShaderResources(pipeline);

	auto rasterizerState = pipeline->rasterizerState()->get();
	if (rasterizerState != m_State.RasterizerState)
	{
		m_State.RasterizerState = rasterizerState;
		m_State.setDirty(D3D11PipelineState::eDirtyTag::eRasterizerState, true);
	}

	auto depthStencilState = pipeline->depthStencilState()->get();
	if (depthStencilState != m_State.DepthStencilState)
	{
		m_State.DepthStencilState = depthStencilState;
		m_State.setDirty(D3D11PipelineState::eDirtyTag::eDepthStencilState, true);
	}

	auto blendState = pipeline->blendState()->get();
	if (blendState != m_State.BlendState)
	{
		m_State.BlendState = blendState;
		m_State.setDirty(D3D11PipelineState::eDirtyTag::eBlendState, true);
	}

	/// set rendertarget/depthstencil

	/// set viewport/scissor

	m_State.submit(*this);

#undef ShaderCaster
}

void D3D11Context::setVertexIndexBuffers(const D3D11GraphicsPipelinePtr& pipeline)
{
	auto gfxState = pipeline->gfxPipelineState();

	/// set input layout
	auto inputLayout = pipeline->inputLayout();
	if (inputLayout->get() != m_State.InputLayout)
	{
		m_State.InputLayout = inputLayout->get();
		m_State.setDirty(D3D11PipelineState::eDirtyTag::eInputLayout, true);
	}

	auto primitiveTopology = D3D11EnumTranslator::toPrimitiveTopology(gfxState->PrimitiveTopology);
	if (primitiveTopology != m_State.PrimitiveTopology)
	{
		m_State.PrimitiveTopology = primitiveTopology;
		m_State.setDirty(D3D11PipelineState::eDirtyTag::ePrimitiveTopology, true);
	}

	auto vertexBuffer = static_cast<D3D11BufferPtr>(gfxState->VertexBuffer)->get();
	if (vertexBuffer != m_State.VertexBuffers[0u])
	{
		m_State.VertexBuffers[0u] = vertexBuffer;
		m_State.VertexOffsets[0u] = 0u;  /// ??? 
		m_State.VertexStrides[0u] = inputLayout->stride();
		
		m_State.VertexBuffersInUse[0] = 1u;
		m_State.setDirty(D3D11PipelineState::eDirtyTag::eVertexBuffer, true);
	}

	auto indexBuffer = static_cast<D3D11BufferPtr>(gfxState->IndexBuffer)->get();
	if (indexBuffer != m_State.IndexBuffer)
	{
		m_State.IndexBuffer = indexBuffer;
		m_State.IndexOffset = 0u; /// ???
		m_State.IndexFormat = gfxState->IndexType == eRIndexType::eUInt16 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;

		m_State.setDirty(D3D11PipelineState::eDirtyTag::eIndexBuffer, true);
	}
}

void D3D11Context::setShaderResources(const D3D11GraphicsPipelinePtr& pipeline)
{
#if !defined(UsingUnorderedMap)
	auto& resourceMap = pipeline->gfxPipelineState()->ResourceMap;

	uint32_t maxBinding = 0u;
	for (auto& res : resourceMap[eVertexShader])
	{
		maxBinding = std::max<uint32_t>(maxBinding, res.Binding);
	}

	for (uint32_t i = 0u; i < eRShaderUsage_MaxEnum; ++i)
	{
		auto& resources = resourceMap[i];
		for (auto& res : resources)
		{
			uint32_t slot = i > eVertexShader ? res.Binding - maxBinding : res.Binding;
			switch (res.Type)
			{
			case GfxPipelineState::eTexture:
			{
				auto texture = std::static_pointer_cast<D3D11ShaderResourceView>(res.Texture)->get();
				assert(texture);
				if (m_State.ShaderResourceViews[i][slot] != texture)
				{
					m_State.ShaderResourceViews[i][slot] = texture;
					m_State.ShaderResourceViewsInUse[i][slot] = 1u;
					m_State.ShaderResourceViewMaxSlot[i] = std::max<uint32_t>(m_State.ShaderResourceViewMaxSlot[i], slot);
					m_State.setDirty(D3D11PipelineState::eDirtyTag::eShaderResourceView, true, i);
				}
			}
				break;
			case GfxPipelineState::eSampler:
			{
				auto sampler = std::static_pointer_cast<D3D11SamplerState>(res.Sampler)->get();
				assert(sampler);
				if (m_State.Samplers[i][slot] != sampler)
				{
					m_State.Samplers[i][slot] = sampler;
					m_State.SamplersInUse[i][slot] = 1u;
					m_State.SamplerMaxSlot[i] = std::max<uint32_t>(m_State.SamplerMaxSlot[i], slot);
					m_State.setDirty(D3D11PipelineState::eDirtyTag::eSampler, true, i);
				}
			}
				break;
			case GfxPipelineState::eCombinedTextureSampler:
			{
				auto texture = std::static_pointer_cast<D3D11ShaderResourceView>(res.Texture)->get();
				assert(texture);
				if (m_State.ShaderResourceViews[i][slot] != texture)
				{
					m_State.ShaderResourceViews[i][slot] = texture;
					m_State.ShaderResourceViewsInUse[i][slot] = 1u;
					m_State.ShaderResourceViewMaxSlot[i] = std::max<uint32_t>(m_State.ShaderResourceViewMaxSlot[i], slot);
					m_State.setDirty(D3D11PipelineState::eDirtyTag::eShaderResourceView, true, i);
				}

				auto sampler = std::static_pointer_cast<D3D11SamplerState>(res.Sampler)->get();
				assert(sampler);
				if (m_State.Samplers[i][slot] != sampler)
				{
					m_State.Samplers[i][slot] = sampler;
					m_State.SamplersInUse[i][slot] = 1u;
					m_State.SamplerMaxSlot[i] = std::max<uint32_t>(m_State.SamplerMaxSlot[i], slot);
					m_State.setDirty(D3D11PipelineState::eDirtyTag::eSampler, true, i);
				}
			}
				break;
			case GfxPipelineState::eUniformBuffer:
			{
				auto uniformBuffer = (static_cast<D3D11BufferPtr>(res.UniformBuffer))->get();
				assert(uniformBuffer);
				if (m_State.UniformBuffers[i][slot] != uniformBuffer)
				{
					m_State.UniformBuffers[i][slot] = uniformBuffer;
					m_State.UniformBuffersInUse[i][slot] = 1u;
					m_State.UniformBufferMaxSlot[i] = std::max<uint32_t>(m_State.UniformBufferMaxSlot[i], slot);
					m_State.setDirty(D3D11PipelineState::eDirtyTag::eUniformBuffer, true, i);
				}
			}
				break;
			}
		}
	}
#else
	assert(0);
#endif
}
#endif
