#include "AppParticleSystem.h"

#include "Camera.h"

extern D3DGraphic* g_Renderer;

void AppParticleSystem::SetupScene()
{
	m_ParticleSystem.Init();

	g_Renderer->SetRenderTarget(g_Renderer->DefaultRenderTarget());
	g_Renderer->SetDepthStencil(g_Renderer->DefaultDepthStencil());

	D3D11_VIEWPORT vp{ 0 };
	vp.Width = (float)m_Width;
	vp.Height = (float)m_Height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = vp.TopLeftY = 0.0f;
	g_Renderer->SetViewports(&vp);

	m_Camera->SetViewRadius(5.0f);
}

void AppParticleSystem::RenderScene()
{
	g_Renderer->ClearRenderTarget(g_Renderer->DefaultRenderTarget());
	g_Renderer->ClearDepthStencil(g_Renderer->DefaultDepthStencil(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0U);

	m_ParticleSystem.Draw(*m_Camera);

	static int32_t s_ParticleType = 0;
	ImGui::Combo("ParticleType", &s_ParticleType, "Fire\0Rain");
	m_ParticleSystem.SetType((ParticleSystem::eParticleType)s_ParticleType);

	static bool s_Wireframe = false;
	ImGui::Checkbox("Wireframe", &s_Wireframe);
	m_ParticleSystem.EnableWireframe(s_Wireframe);
}

void AppParticleSystem::UpdateScene(float elapseTime, float totalTime)
{
	Base::UpdateScene(elapseTime, totalTime);

	m_ParticleSystem.Update(elapseTime, totalTime);
}
