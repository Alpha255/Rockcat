#include "Box.h"
#include "Public/ImGUI.h"

void Box::PrepareScene()
{
	m_Box.CreateAsCube(1.0f, 1.0f, 1.0f);
	m_CBufferVS.CreateAsUniformBuffer(sizeof(Matrix), eGpuReadCpuWrite, nullptr);

	m_DiffuseTex.Create("WoodCrate01.dds");

	m_VS.Create("Box.shader", "VSMain");

	m_PS[eNone].Create("Box.shader", "PSMain");
	m_PS[eInversion].Create("Box.shader", "PSMain_Inversion");
	m_PS[eGrayscale].Create("Box.shader", "PSMain_Grayscale");
	m_PS[eSharpen].Create("Box.shader", "PSMain_Sharpen");
	m_PS[eBlur].Create("Box.shader", "PSMain_Blur");
	m_PS[eEdgeDetection].Create("Box.shader", "PSMain_EdgeDetection");

	AutoFocus(m_Box, 2.0f);
}

void Box::RenderScene()
{
	REngine::Instance().ResetDefaultRenderSurfaces();

	if (m_bWireframe)
	{
		REngine::Instance().SetRasterizerState(RStaticState::Wireframe);
	}

	REngine::Instance().SetViewport(RViewport(0.0f, 0.0f, (float)m_WindowSize.first, (float)m_WindowSize.second));

	REngine::Instance().SetVertexShader(m_VS);
	REngine::Instance().SetPixelShader(m_PS[m_Effect]);

	Matrix wvp = Matrix::Transpose(m_Camera.GetWVPMatrix());
	m_CBufferVS.Update(&wvp, sizeof(Matrix));
	REngine::Instance().SetUniformBuffer(m_CBufferVS, 0U, eVertexShader);

	REngine::Instance().SetShaderResourceView(m_DiffuseTex, 0U, ePixelShader);
	REngine::Instance().SetSamplerState(RStaticState::LinearSampler, 0U, ePixelShader);

	m_Box.Draw(m_Camera);

	ImGui::Combo("SpecialEffect", &m_Effect, "None\0Inversion\0Grayscale\0Sharpen\0Blur\0EdgeDetection");
	ImGui::Checkbox("VSync", &m_bVSync);
	ImGui::Checkbox("Wireframe", &m_bWireframe);
	ImGui::Text("\n%.2f FPS", m_FPS);
}