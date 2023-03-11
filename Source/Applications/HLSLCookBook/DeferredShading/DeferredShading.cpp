#include "DeferredShading.h"
#include "Base/DXUTCamera.h"
#include "Colorful/Public/ImGUI.h"

void DeferredShading::PrepareScene()
{
	m_Bunny.CreateFromFile("bunny.sdkmesh");

	m_GBuffer.Init(m_WindowSize.first, m_WindowSize.second);
	m_LightController.Init();

	m_VertexShader.Create("DeferredShading\\DeferredShading.hlsl", "VSMain");
	m_PixelShader.Create("DeferredShading\\DeferredShading.hlsl", "PSMain");

	m_CBufferVS.CreateAsUniformBuffer(sizeof(ConstantBufferVS), eGpuReadCpuWrite);
	m_CBufferPS.CreateAsUniformBuffer(sizeof(ConstantBufferPS), eGpuReadCpuWrite);
}

void DeferredShading::RenderScene()
{
	ConstantBufferVS CBufferVS;
	ConstantBufferPS CBufferPS;

    REngine::Instance().SetViewport(RViewport(0.0f, 0.0f, (float)m_WindowSize.first, (float)m_WindowSize.second));

	CBufferVS.World = Matrix::Transpose(m_Camera.GetWorldMatrix());
	CBufferVS.WVP = Matrix::Transpose(m_Camera.GetWVPMatrix());
	m_CBufferVS.Update(&CBufferVS, sizeof(ConstantBufferVS));

	CBufferPS.LightSpecular = m_LightController.GetLightSpecular((eLightType)m_LightingType);
	m_CBufferPS.Update(&CBufferPS, sizeof(ConstantBufferPS));

	REngine::Instance().SetVertexShader(m_VertexShader);
	REngine::Instance().SetPixelShader(m_PixelShader);
	REngine::Instance().SetUniformBuffer(m_CBufferVS, 0U, eVertexShader);
	REngine::Instance().SetUniformBuffer(m_CBufferPS, 0U, ePixelShader);

	m_GBuffer.Bind();
	m_Bunny.Draw(m_Camera);
	m_GBuffer.UnBind();

	REngine::Instance().ResetDefaultRenderTargetView();

	Matrix proj = m_Camera.GetProjMatrix();
	Vec4 perspective = Vec4(1.0f / proj._11, 1.0f / proj._22, proj._43, -proj._33);

	m_LightController.TurnonTheLights((eLightType)m_LightingType, m_GBuffer, perspective, m_Camera.GetViewMatrix());

	m_GBuffer.VisulizeGBuffer(m_bVisualizeGBuffer, perspective);

	ImGui::Checkbox("VisualizeGBuffer", &m_bVisualizeGBuffer);
	ImGui::Combo("LightingType", &m_LightingType, "Point\0Directional\0Spot\0Capsule");
}

void DeferredShading::ResizeWindow(uint32_t width, uint32_t height)
{
	BaseClass::ResizeWindow(width, height);

	m_GBuffer.Resize(width, height);
}
