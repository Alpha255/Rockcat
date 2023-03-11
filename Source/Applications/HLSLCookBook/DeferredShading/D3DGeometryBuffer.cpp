#include "D3DGeometryBuffer.h"

void D3DGeometryBuffer::Init(uint32_t width, uint32_t height, bool bReinit)
{
	RTexture2D depthTex;
	depthTex.Create(eR24G8_Typeless, width, height, eBindAsDepthStencil | eBindAsShaderResource, 1U);
	m_SurfaceDepthStencil.CreateAsTexture(eTexture2D, depthTex, eD24_UNorm_S8_UInt, 0U, 0U);
	m_SurfaceDepthStencilReadonly.CreateAsTexture(eTexture2D, depthTex, eD24_UNorm_S8_UInt, eReadonlyDepthStencil, 0U);
	m_ShaderResourceViews[eDepthStencil].CreateAsTexture(eTexture2D, depthTex, eR24_UNorm_X8_Typeless, 0U, 1U);

	RTexture2D colorTex;
	colorTex.Create(eRGBA8_UNorm, width, height, eBindAsRenderTarget | eBindAsShaderResource, 1U);
	m_SurfaceColorSpecIntensity.CreateAsTexture(eTexture2D, colorTex, eRGBA8_UNorm, 0U);
	m_ShaderResourceViews[eColorSpecIntensity].CreateAsTexture(eTexture2D, colorTex, eRGBA8_UNorm, 0U, 1U);

	RTexture2D normalTex;
	normalTex.Create(eRG11B10_Float, width, height, eBindAsRenderTarget | eBindAsShaderResource, 1U);
	m_SurfaceNormal.CreateAsTexture(eTexture2D, normalTex, eRG11B10_Float, 0U);
	m_ShaderResourceViews[eNormal].CreateAsTexture(eTexture2D, normalTex, eRG11B10_Float, 0U, 1U);

	RTexture2D specPowerTex;
	specPowerTex.Create(eRGBA8_UNorm, width, height, eBindAsRenderTarget | eBindAsShaderResource, 1U);
	m_SurfaceSpecPower.CreateAsTexture(eTexture2D, specPowerTex, eRGBA8_UNorm, 0U);
	m_ShaderResourceViews[eSpecPower].CreateAsTexture(eTexture2D, specPowerTex, eRGBA8_UNorm, 0U, 1U);

	if (bReinit)
	{
		return;
	}

	m_DepthStencilState.Create(
		true, eDepthMaskAll, eRComparisonFunc::eLess,
		true, eStencilDefaultReadMask, eStencilDefaultWriteMask,
		eRStencilOp::eReplace, eRStencilOp::eReplace, eRStencilOp::eReplace, eRComparisonFunc::eAlways,
		eRStencilOp::eReplace, eRStencilOp::eReplace, eRStencilOp::eReplace, eRComparisonFunc::eAlways);

	m_VertexShader.Create("DeferredShading\\VisualizeGBuffer.hlsl", "VSMain");
	m_PixelShader.Create("DeferredShading\\VisualizeGBuffer.hlsl", "PSMain");

	m_CBufferPS.CreateAsUniformBuffer(sizeof(ConstantBufferPS), eGpuReadCpuWrite);
}

void D3DGeometryBuffer::Resize(uint32_t width, uint32_t height)
{
	m_SurfaceColorSpecIntensity.Reset();
	m_SurfaceNormal.Reset();
	m_SurfaceSpecPower.Reset();

	m_SurfaceDepthStencil.Reset();
	m_SurfaceDepthStencilReadonly.Reset();

	for (uint32_t i = 0U; i < eBufferTypeCount; ++i)
	{
		m_ShaderResourceViews[i].Reset();
	}

	Init(width, height, true);
}

void D3DGeometryBuffer::Bind()
{
	REngine::Instance().ClearDepthStencilView(m_SurfaceDepthStencil);

	REngine::Instance().ClearRenderTargetView(m_SurfaceColorSpecIntensity, Color::Black);
	REngine::Instance().ClearRenderTargetView(m_SurfaceNormal, Color::Black);
	REngine::Instance().ClearRenderTargetView(m_SurfaceSpecPower, Color::Black);

	REngine::Instance().SetRenderTargetView(m_SurfaceColorSpecIntensity, 0U);
	REngine::Instance().SetRenderTargetView(m_SurfaceNormal, 1U);
	REngine::Instance().SetRenderTargetView(m_SurfaceSpecPower, 2U);

	REngine::Instance().SetDepthStencilView(m_SurfaceDepthStencil);

	REngine::Instance().SetDepthStencilState(m_DepthStencilState, 0x01U);
}

void D3DGeometryBuffer::UnBind()
{
	RRenderTargetView EmptyRTV;
	REngine::Instance().SetRenderTargetView(EmptyRTV, 0U);
	REngine::Instance().SetRenderTargetView(EmptyRTV, 1U);
	REngine::Instance().SetRenderTargetView(EmptyRTV, 2U);

	REngine::Instance().SetDepthStencilView(m_SurfaceDepthStencilReadonly);

	RShaderResourceView EmptySRV;
	for (uint32_t i = 0U; i < eBufferTypeCount; ++i)
	{
		REngine::Instance().SetShaderResourceView(EmptySRV, i, ePixelShader);
	}

	///REngine::Instance().ForceCommitState();
}

void D3DGeometryBuffer::VisulizeGBuffer(bool bVisulize, const Vec4 &camPerspective)
{
	if (bVisulize)
	{
		RDepthStencilView EmptyDSV;
		REngine::Instance().SetDepthStencilView(EmptyDSV);

		RInputLayout EmptyLayout;
		RBuffer EmptyVertexBuffer;
		REngine::Instance().SetInputLayout(EmptyLayout);
		REngine::Instance().SetVertexBuffer(EmptyVertexBuffer, 0U, 0U, 0U);

		REngine::Instance().SetVertexShader(m_VertexShader);
		REngine::Instance().SetPixelShader(m_PixelShader);

		ConstantBufferPS CBufferPS;
		CBufferPS.Perspective = camPerspective;
		m_CBufferPS.Update(&CBufferPS, sizeof(ConstantBufferPS));
		REngine::Instance().SetUniformBuffer(m_CBufferPS, 0U, ePixelShader);

		REngine::Instance().SetSamplerState(RStaticState::PointSampler, 0U, ePixelShader);

		for (uint32_t i = 0U; i < eBufferTypeCount; ++i)
		{
			REngine::Instance().SetShaderResourceView(m_ShaderResourceViews[i], i, ePixelShader);
		}

		REngine::Instance().Draw(16U, 0U, eTriangleStrip);
	}

	RShaderResourceView EmptySRV;
	for (uint32_t i = 0U; i < eBufferTypeCount; ++i)
	{
		REngine::Instance().SetShaderResourceView(EmptySRV, i, ePixelShader);
	}
}