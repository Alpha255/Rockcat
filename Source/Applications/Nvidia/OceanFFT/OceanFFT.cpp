#include "OceanFFT.h"

#include "D3DGraphic.h"
#include "Camera.h"
#include "SkyDome.h"

extern D3DGraphic* g_Renderer;

static SkyDome s_SkyDome;

void AppOceanFFT::SetupScene()
{
	g_Renderer->SetRenderTarget(g_Renderer->DefaultRenderTarget());
	g_Renderer->SetDepthStencil(g_Renderer->DefaultDepthStencil());

	D3D11_VIEWPORT vp{ 0 };
	vp.Width = (float)m_Width;
	vp.Height = (float)m_Height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = vp.TopLeftY = 0.0f;
	g_Renderer->SetViewports(&vp);

	///m_Camera->SetViewParams(Vec3(-2800.0f, 1000.0f, 0.0f), Vec3(0.0f, -1000.0f, -1000.0f));
	m_Camera->SetViewRadius(1.0f);

	s_SkyDome.Create(m_Width, m_Height);
}

void AppOceanFFT::RenderScene()
{
	g_Renderer->ClearRenderTarget(g_Renderer->DefaultRenderTarget(), reinterpret_cast<const float*>(&Color::DarkBlue));
	g_Renderer->ClearDepthStencil(g_Renderer->DefaultDepthStencil(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0U);

	s_SkyDome.Draw(*m_Camera);
}

void AppOceanFFT::ResizeWindow(uint32_t width, uint32_t height)
{
	Base::ResizeWindow(width, height);

	s_SkyDome.Resize(width, height);
}
