#include "AlphaBlend.h"
#include "Camera.h"
#include "D3DEngine.h"
#include "ImGUI.h"

void AppAlphaBlend::Initialize()
{
	m_VertexShader.Create("AlphaBlend.hlsl", "VSMain");
	m_PixelShader.Create("AlphaBlend.hlsl", "PSMain");

	m_FloorMesh.CreateAsQuad(10.0f);
	m_CubeMesh.CreateAsCube(1.0f);
	m_WindowMesh.CreateAsQuad(1.0f);

	m_TexFloor.Create("toy_box_diffuse.dds");
	m_TexCube.Create("wood.dds");
	m_TexWindow.Create("window.dds");

	m_CBufferVS.CreateAsConstantBuffer(sizeof(Matrix), D3DBuffer::eGpuReadCpuWrite, nullptr);

	m_AlphaBlend.Create(false, false, 0U, true,
		D3DState::eSrcAlpha, D3DState::eInvSrcAlpha, D3DState::eAdd,
		D3DState::eOne, D3DState::eZero, D3DState::eAdd,
		D3DState::eColorAll);

	m_Camera->SetViewRadius(15.0f);
}

void AppAlphaBlend::RenderScene()
{
	D3DEngine::Instance().ResetDefaultRenderSurfaces();
	D3DEngine::Instance().SetViewport(D3DViewport(0.0f, 0.0f, (float)m_Width, (float)m_Height));

	D3DEngine::Instance().SetInputLayout(m_FloorMesh.VertexLayout);

	D3DEngine::Instance().SetVertexShader(m_VertexShader);
	D3DEngine::Instance().SetPixelShader(m_PixelShader);

	D3DEngine::Instance().SetSamplerState(D3DStaticState::LinearSampler, 0U, D3DShader::ePixelShader);

	/// Draw floor
	D3DEngine::Instance().SetVertexBuffer(m_FloorMesh.VertexBuffer, sizeof(Geometry::Vertex), 0U);
	D3DEngine::Instance().SetIndexBuffer(m_FloorMesh.IndexBuffer, eR32_UInt, 0U);
	Matrix wvp = Matrix::Transpose(m_Camera->GetWVPMatrix());
	m_CBufferVS.Update(&wvp, sizeof(Matrix));
	D3DEngine::Instance().SetConstantBuffer(m_CBufferVS, 0U, D3DShader::eVertexShader);
	D3DEngine::Instance().SetShaderResourceView(m_TexFloor, 0U, D3DShader::ePixelShader);
	D3DEngine::Instance().SetRasterizerState(D3DStaticState::SolidNoneCulling);
	D3DEngine::Instance().DrawIndexed(6U, 0U, 0, eTriangleList);

	/// Draw cube0
	Matrix translation = Matrix::Translation(-2.0f, 0.0f, -0.51f);
	wvp = Matrix::Transpose(m_Camera->GetWorldMatrix() * translation * m_Camera->GetViewMatrix() * m_Camera->GetProjMatrix());
	m_CBufferVS.Update(&wvp, sizeof(Matrix));
	D3DEngine::Instance().SetVertexBuffer(m_CubeMesh.VertexBuffer, sizeof(Geometry::Vertex), 0U, 0U);
	D3DEngine::Instance().SetIndexBuffer(m_CubeMesh.IndexBuffer, eR32_UInt, 0U);
	D3DEngine::Instance().SetRasterizerState(D3DStaticState::Solid);
	D3DEngine::Instance().SetShaderResourceView(m_TexCube, 0U, D3DShader::ePixelShader);
	D3DEngine::Instance().DrawIndexed(36U, 0U, 0, eTriangleList);

	/// Draw cube1
	translation = Matrix::Translation(2.0f, 1.0f, -0.51f);
	wvp = Matrix::Transpose(m_Camera->GetWorldMatrix() * translation * m_Camera->GetViewMatrix() * m_Camera->GetProjMatrix());
	m_CBufferVS.Update(&wvp, sizeof(Matrix));
	D3DEngine::Instance().DrawIndexed(36U, 0U, 0, eTriangleList);

	/// Draw window0
	Matrix rotate = Matrix::RotationAxis(1.0f, 0.0f, 0.0f, 90.0f);
	translation = Matrix::Translation(2.0f, 0.49f, -0.51f);
	wvp = Matrix::Transpose(m_Camera->GetWorldMatrix() * rotate * translation * m_Camera->GetViewMatrix() * m_Camera->GetProjMatrix());
	m_CBufferVS.Update(&wvp, sizeof(Matrix));
	D3DEngine::Instance().SetVertexBuffer(m_WindowMesh.VertexBuffer, sizeof(Geometry::Vertex), 0U, 0U);
	D3DEngine::Instance().SetIndexBuffer(m_WindowMesh.IndexBuffer, eR32_UInt, 0U);
	D3DEngine::Instance().SetShaderResourceView(m_TexWindow, 0U, D3DShader::ePixelShader);
	D3DEngine::Instance().SetRasterizerState(D3DStaticState::SolidNoneCulling);
	if (m_bAlphaBlend)
	{
		D3DEngine::Instance().SetBlendState(m_AlphaBlend);
	}
	D3DEngine::Instance().DrawIndexed(6U, 0U, 0, eTriangleList);

	/// Draw window1
	translation = Matrix::Translation(-2.0f, -0.51f, -0.51f);
	wvp = Matrix::Transpose(m_Camera->GetWorldMatrix() * rotate * translation * m_Camera->GetViewMatrix() * m_Camera->GetProjMatrix());
	m_CBufferVS.Update(&wvp, sizeof(Matrix));
	D3DEngine::Instance().DrawIndexed(6U, 0U, 0, eTriangleList);

	/// Draw overlay windows
	float drawOrder[3] = { 0.8f, 0.3f, -0.2f };
	for (uint32_t i = 0U; i < 3U; ++i)
	{
		translation = Matrix::Translation(-0.5f * ((i + 1) % 2 == 0 ? 1.0f : 0.0f), drawOrder[i], -0.51f);
		wvp = Matrix::Transpose(m_Camera->GetWorldMatrix() * rotate * translation * m_Camera->GetViewMatrix() * m_Camera->GetProjMatrix());
		m_CBufferVS.Update(&wvp, sizeof(Matrix));
		D3DEngine::Instance().DrawIndexed(6U, 0U, 0, eTriangleList);
	}

	ImGui::Checkbox("AlphaBlend", &m_bAlphaBlend);
}