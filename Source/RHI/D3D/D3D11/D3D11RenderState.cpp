#include "Colorful/D3D/D3D11/D3D11RenderState.h"

NAMESPACE_START(Gfx)

NAMESPACE_END(Gfx)

#if 0
#include "Colorful/D3D/D3D11/D3D11Pipeline.h"

void D3D11PipelineState::submit(D3D11Context& context)
{
	if (!context.isValid())
	{
		return;
	}

	/// Input Assembler
	if (isDirty(eDirtyTag::eInputLayout))
	{
		context->IASetInputLayout(InputLayout);
		setDirty(eDirtyTag::eInputLayout, false);
	}

	if (isDirty(eDirtyTag::eVertexBuffer))
	{
		assert(VertexBuffersInUse.any());
		context->IASetVertexBuffers(0u, (uint32_t)VertexBuffersInUse.count(), VertexBuffers.data(), VertexStrides.data(), VertexOffsets.data());
		setDirty(eDirtyTag::eVertexBuffer, false);
	}

	if (isDirty(eDirtyTag::eIndexBuffer))
	{
		context->IASetIndexBuffer(IndexBuffer, IndexFormat, IndexOffset);
		setDirty(eDirtyTag::eIndexBuffer, false);
	}

	if (isDirty(eDirtyTag::ePrimitiveTopology))
	{
		context->IASetPrimitiveTopology(PrimitiveTopology);
		setDirty(eDirtyTag::ePrimitiveTopology, false);
	}

	setUniformBuffers(context);
	setSamplers(context);
	setShaderResourceViews(context);
	setUnorderedAccessViews(context);

	/// VS->HS->DS->GS
	if (isDirty(eDirtyTag::eVertexShader))
	{
		assert(VertexShader);
		context->VSSetShader(VertexShader, nullptr, 0u);
		setDirty(eDirtyTag::eVertexShader, false);
	}

	if (isDirty(eDirtyTag::eHullShader))
	{
		context->HSSetShader(HullShader, nullptr, 0u);
		setDirty(eDirtyTag::eHullShader, false);
	}

	if (isDirty(eDirtyTag::eDomainShader))
	{
		context->DSSetShader(DomainShader, nullptr, 0u);
		setDirty(eDirtyTag::eDomainShader, false);
	}

	if (isDirty(eDirtyTag::eGeometryShader))
	{
		context->GSSetShader(GeometryShader, nullptr, 0u);
		setDirty(eDirtyTag::eGeometryShader, false);
	}

	/// Rasterizer
	if (isDirty(eDirtyTag::eRasterizerState))
	{
		context->RSSetState(RasterizerState);
		setDirty(eDirtyTag::eRasterizerState, false);
	}

	if (isDirty(eDirtyTag::eViewport))
	{
		assert(ViewportsInUse.any());
		context->RSSetViewports((uint32_t)ViewportsInUse.count(), Viewports.data());
		setDirty(eDirtyTag::eViewport, false);
	}

	if (isDirty(eDirtyTag::eScissorRect))
	{
		context->RSSetScissorRects((uint32_t)ScissorRectsInUse.count(), ScissorRects.data());
		setDirty(eDirtyTag::eScissorRect, false);
	}

	/// Fragment Shader
	if (isDirty(eDirtyTag::ePixelShader))
	{
		context->PSSetShader(PixelShader, nullptr, 0u);
		setDirty(eDirtyTag::ePixelShader, false);
	}

	if (isDirty(eDirtyTag::eComputeShader))
	{
		context->CSSetShader(ComputeShader, nullptr, 0u);
		setDirty(eDirtyTag::eComputeShader, false);
	}

	/// Output Merge
	if (isDirty(eDirtyTag::eBlendState))
	{
		context->OMSetBlendState(BlendState, (float32_t*)&BlendFactor, BlendMask);
		setDirty(eDirtyTag::eBlendState, false);
	}

	if (isDirty(eDirtyTag::eDepthStencilState))
	{
		context->OMSetDepthStencilState(DepthStencilState, StencilRef);
		setDirty(eDirtyTag::eDepthStencilState, false);
	}

	if (isDirty(eDirtyTag::eRenderTargetView) || isDirty(eDirtyTag::eDepthStencilView))
	{
		context->OMSetRenderTargets((uint32_t)RenderTargetsInUse.count(), RenderTargets.data(), DepthStencilView);
		///SetDirty(eRenderTargetView, false);
		setDirty(eDirtyTag::eDepthStencilView, false);
	}
}

void D3D11PipelineState::setUniformBuffers(D3D11Context& context)
{
	for (uint32_t i = 0u; i < eRShaderUsage_MaxEnum; ++i)
	{
		if (isDirty(eDirtyTag::eUniformBuffer, i))
		{
			if (0u == UniformBuffersInUse[i].count())
			{
				continue;
			}

			uint32_t constantBuffers = UniformBufferMaxSlot[i] + 1u;
			switch (i)
			{
			case eVertexShader:   context->VSSetConstantBuffers(0u, constantBuffers, UniformBuffers[i].data()); break;
			case eHullShader:     context->HSSetConstantBuffers(0u, constantBuffers, UniformBuffers[i].data()); break;
			case eDomainShader:   context->DSSetConstantBuffers(0u, constantBuffers, UniformBuffers[i].data()); break;
			case eGeometryShader: context->GSSetConstantBuffers(0u, constantBuffers, UniformBuffers[i].data()); break;
			case eFragmentShader: context->PSSetConstantBuffers(0u, constantBuffers, UniformBuffers[i].data()); break;
			case eComputeShader:  context->CSSetConstantBuffers(0u, constantBuffers, UniformBuffers[i].data()); break;
			}

			setDirty(eDirtyTag::eUniformBuffer, false);
		}
	}
}

void D3D11PipelineState::setSamplers(D3D11Context& context)
{
	for (uint32_t i = 0u; i < eRShaderUsage_MaxEnum; ++i)
	{
		if (isDirty(eDirtyTag::eSampler, i))
		{
			if (0u == SamplersInUse[i].count())
			{
				continue;
			}

			uint32_t samplers = SamplerMaxSlot[i] + 1u;
			switch (i)
			{
			case eVertexShader:   context->VSSetSamplers(0u, samplers, Samplers[i].data()); break;
			case eHullShader:     context->HSSetSamplers(0u, samplers, Samplers[i].data()); break;
			case eDomainShader:   context->DSSetSamplers(0u, samplers, Samplers[i].data()); break;
			case eGeometryShader: context->GSSetSamplers(0u, samplers, Samplers[i].data()); break;
			case eFragmentShader: context->PSSetSamplers(0u, samplers, Samplers[i].data()); break;
			case eComputeShader:  context->CSSetSamplers(0u, samplers, Samplers[i].data()); break;
			}
		}

		setDirty(eDirtyTag::eSampler, false, i);
	}
}

void D3D11PipelineState::setShaderResourceViews(D3D11Context& context)
{
	for (uint32_t i = 0u; i < eRShaderUsage_MaxEnum; ++i)
	{
		if (isDirty(eDirtyTag::eShaderResourceView, i))
		{
			if (0u == ShaderResourceViewsInUse[i].count())
			{
				continue;
			}

			uint32_t shaderResourceViews = ShaderResourceViewMaxSlot[i] + 1u;
			switch (i)
			{
			case eVertexShader:   context->VSSetShaderResources(0u, shaderResourceViews, ShaderResourceViews[i].data()); break;
			case eHullShader:     context->HSSetShaderResources(0u, shaderResourceViews, ShaderResourceViews[i].data()); break;
			case eDomainShader:   context->DSSetShaderResources(0u, shaderResourceViews, ShaderResourceViews[i].data()); break;
			case eGeometryShader: context->GSSetShaderResources(0u, shaderResourceViews, ShaderResourceViews[i].data()); break;
			case eFragmentShader: context->PSSetShaderResources(0u, shaderResourceViews, ShaderResourceViews[i].data()); break;
			case eComputeShader:  context->CSSetShaderResources(0u, shaderResourceViews, ShaderResourceViews[i].data()); break;
			}

			setDirty(eDirtyTag::eShaderResourceView, false, i);
		}
	}
}

void D3D11PipelineState::setUnorderedAccessViews(D3D11Context& context)
{
	if (!isDirty(eDirtyTag::eUnorderedAccessView))
	{
		return;
	}

	uint32_t unorderedAccessViews = (uint32_t)UnorderedAccessViewsInUse.count();

	if (unorderedAccessViews > 0u)
	{
		context->CSSetUnorderedAccessViews(0u, unorderedAccessViews, UnorderedAccessViews.data(), nullptr);
	}

	setDirty(eDirtyTag::eUnorderedAccessView, false);
}
#endif