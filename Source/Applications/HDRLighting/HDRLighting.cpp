#include "HDRLighting.h"
#include "D3DEngine.h"
#include "Camera.h"

void AppHDRLighting::Initialize()
{
	m_Room.CreateAsBox(15.0f, 3.0f, 20.0f);

	m_VertexShader.Create("HDRLighting.hlsl", "VSMain");
	m_PixelShader.Create("HDRLighting.hlsl", "PSMain");

	m_CBufferVS.CreateAsConstantBuffer(sizeof(Matrix), D3DBuffer::eGpuReadCpuWrite);

	m_DiffuseTex.Create("brick01.dds");
}

void AppHDRLighting::RenderScene()
{
	D3DEngine::Instance().ResetDefaultRenderSurfaces();
	D3DEngine::Instance().SetViewport(D3DViewport(0.0f, 0.0f, (float)m_Width, (float)m_Height));

	D3DEngine::Instance().SetVertexShader(m_VertexShader);
	D3DEngine::Instance().SetPixelShader(m_PixelShader);
	D3DEngine::Instance().SetConstantBuffer(m_CBufferVS, 0U, D3DShader::eVertexShader);

	Matrix wvp = Matrix::Transpose(m_Camera->GetWVPMatrix());
	m_CBufferVS.Update(&wvp, sizeof(Matrix));

	D3DEngine::Instance().SetRasterizerState(D3DStaticState::SolidNoneCulling);
	D3DEngine::Instance().SetShaderResourceView(m_DiffuseTex, 0U, D3DShader::ePixelShader);
	D3DEngine::Instance().SetSamplerState(D3DStaticState::LinearSampler, 0U, D3DShader::ePixelShader);

	m_Room.Draw();
}
