#include "DepthStencilTest.h"
#include "Camera.h"
#include "D3DEngine.h"
#include "D3DLighting.h"
#include "ImGUI.h"

void AppDepthStencilTest::Initialize()
{
	///   |--------------|
	///   |              |
	///   |----|----|----|
	///   |Wall|Mirr|Wall|
	///   |    | or |    |
	///   /--------------/
	///  /   Floor      /
	/// /--------------/
	m_RoomMesh.Create("Room.obj");
	m_SphereMesh.CreateAsGeoSphere(1.0f, 3U);

	m_FloorTex.Create("checkboard.dds");
	m_WallTex.Create("brick01.dds");
	m_MirrorTex.Create("ice.dds");
	m_SphereTex.Create("wood.dds");

	m_VertexShader.Create("DepthStencilTest.hlsl", "VSMain");
	m_PixelShader.Create("DepthStencilTest.hlsl", "PSMain");

	m_CBufferVS.CreateAsConstantBuffer(sizeof(Matrix), D3DBuffer::eGpuReadCpuWrite);
	m_CBufferPS.CreateAsConstantBuffer(sizeof(Vec4), D3DBuffer::eGpuReadCpuWrite);

	/// src(sr, sg, sb, sa)  dst(dr, dg, db, da)
	/// output(r, g, b) = 1 * (sr, sg, sb) + 0 * (dr, dg, db)
	/// outout(a) = 1 * sa + 0 * da
	m_NoColorWrite.Create(false, false, 0U, false,
		D3DState::eOne, D3DState::eZero, D3DState::eAdd,
		D3DState::eOne, D3DState::eZero, D3DState::eAdd,
		D3DState::eColorNone);

	/// src(sr, sg, sb, sa)  dst(dr, dg, db, da)
	/// output(r, g, b) = sa * (sr, sg, sb) + (1 - sa) * (dr, dg, db)
	/// output(a) = 1 * sa + 0 * da
	m_AlphaBlend.Create(false, false, 0U, true,
		D3DState::eSrcAlpha, D3DState::eInvSrcAlpha, D3DState::eAdd,
		D3DState::eOne, D3DState::eZero, D3DState::eAdd,
		D3DState::eColorAll);

	/// (StencilRef & StencilReadMask) -CompareFunc- (Value & StencilReadMask)
	m_MarkMirror.Create(true, D3DState::eDepthMaskZero, D3DState::eLess,
		true, D3DState::eStencilDefaultReadMask, D3DState::eStencilDefaultWriteMask,
		D3DState::eStencilKeep, D3DState::eStencilKeep, D3DState::eStencilReplace, D3DState::eAlways,
		D3DState::eStencilKeep, D3DState::eStencilKeep, D3DState::eStencilReplace, D3DState::eAlways); /// Only use front face

	m_Reflection.Create(true, D3DState::eDepthMaskAll, D3DState::eLess,
		true, D3DState::eStencilDefaultReadMask, D3DState::eStencilDefaultWriteMask,
		D3DState::eStencilKeep, D3DState::eStencilKeep, D3DState::eStencilKeep, D3DState::eEqual,
		D3DState::eStencilKeep, D3DState::eStencilKeep, D3DState::eStencilKeep, D3DState::eEqual);   /// Only use front face
}

void AppDepthStencilTest::UpdateConstantBufferVS(const Matrix &world)
{
	Matrix wvp = Matrix::Transpose(world * m_Camera->GetViewMatrix() * m_Camera->GetProjMatrix());
	m_CBufferVS.Update(&wvp, sizeof(Matrix));
}

void AppDepthStencilTest::DrawFloorAndWalls(const Matrix &world)
{
	UpdateConstantBufferVS(world);

	Vec4 roomMaterial(0.0f, 0.0f, 0.0f, 1.0f);
	m_CBufferPS.Update(&roomMaterial, sizeof(Vec4));

	m_RoomMesh.Bind();
	D3DEngine::Instance().SetShaderResourceView(m_FloorTex, 0U, D3DShader::ePixelShader);
	D3DEngine::Instance().Draw(6U, 0U, eTriangleList);
	D3DEngine::Instance().SetShaderResourceView(m_WallTex, 0U, D3DShader::ePixelShader);
	D3DEngine::Instance().Draw(18U, 6U, eTriangleList);
}

void AppDepthStencilTest::DrawSphere(const Matrix &world)
{
	UpdateConstantBufferVS(world);

	m_SphereMesh.Bind();
	D3DEngine::Instance().SetShaderResourceView(m_SphereTex, 0U, D3DShader::ePixelShader);
	D3DEngine::Instance().DrawIndexed(m_SphereMesh.IndexCount, 0U, 0, eTriangleList);
}

void AppDepthStencilTest::DrawMirror(const Matrix &world)
{
	UpdateConstantBufferVS(world);

	Vec4 mirrorMaterial(0.0f, 0.0f, 0.0f, 0.5f);
	m_CBufferPS.Update(&mirrorMaterial, sizeof(Vec4));

	m_RoomMesh.Bind();
	D3DEngine::Instance().SetShaderResourceView(m_MirrorTex, 0U, D3DShader::ePixelShader);
	D3DEngine::Instance().Draw(6U, 24U, eTriangleList);
}

void AppDepthStencilTest::RenderScene()
{
	D3DEngine::Instance().ResetDefaultRenderSurfaces();
	D3DEngine::Instance().SetViewport(D3DViewport(0.0f, 0.0f, (float)m_Width, (float)m_Height));

	D3DEngine::Instance().SetVertexShader(m_VertexShader);
	D3DEngine::Instance().SetPixelShader(m_PixelShader);

	D3DEngine::Instance().SetConstantBuffer(m_CBufferVS, 0U, D3DShader::eVertexShader);
	D3DEngine::Instance().SetConstantBuffer(m_CBufferPS, 0U, D3DShader::ePixelShader);

	D3DEngine::Instance().SetSamplerState(D3DStaticState::LinearSampler, 0U, D3DShader::ePixelShader);

	/// Draw floor and walls
	Matrix worldFloorWall = m_Camera->GetWorldMatrix();
	DrawFloorAndWalls(worldFloorWall);

	/// Draw sphere
	Matrix translation = Matrix::Translation(m_SphereTrans.x, 2.0f + m_SphereTrans.y, -2.0f);
	Matrix worldSphere = m_Camera->GetWorldMatrix() * translation;
	DrawSphere(worldSphere);

	/// Draw mirror to stencil buffer, disable depth write, and set stencil value to 1
	/// Do not write mirror depth to depth buffer at this point, otherwise it will occlude the reflection.
	D3DEngine::Instance().SetBlendState(m_NoColorWrite);
	D3DEngine::Instance().SetDepthStencilState(m_MarkMirror, 0xffffffff);
	DrawMirror(worldFloorWall);
	D3DEngine::Instance().SetBlendState(D3DStaticState::NoneBlendState);

	/// Draw sphere reflection, remember reflect light also
	/// Enable depth write, only when stencil value equal to ref value can pass the test
	Matrix reflection = Matrix::Reflect(0.0f, 0.0f, 1.0f);
	worldSphere = worldSphere * reflection;
	D3DEngine::Instance().SetRasterizerState(D3DStaticState::SolidFrontCCW);
	D3DEngine::Instance().SetDepthStencilState(m_Reflection, 0xffffffff);
	DrawSphere(worldSphere);
	D3DEngine::Instance().SetDepthStencilState(D3DStaticState::NoneDepthStencilState, 0U);
	D3DEngine::Instance().SetRasterizerState(D3DStaticState::Solid);

	/// Draw the mirror to the back buffer as usual but with transparency blending so the reflection shows through.
	D3DEngine::Instance().SetBlendState(m_AlphaBlend);
	DrawMirror(worldFloorWall);
	D3DEngine::Instance().SetBlendState(D3DStaticState::NoneBlendState);
}

void AppDepthStencilTest::Update(float elapsedTime, float /*totalTime*/)
{
	if (GetAsyncKeyState('A') & 0x8000)
	{
		m_SphereTrans.x -= 1.0f * elapsedTime;
	}
	if (GetAsyncKeyState('D') & 0x8000)
	{
		m_SphereTrans.x += 1.0f * elapsedTime;
	}
	if (GetAsyncKeyState('W') & 0x8000)
	{
		m_SphereTrans.y += 1.0f * elapsedTime;
	}
	if (GetAsyncKeyState('S') & 0x8000)
	{
		m_SphereTrans.y -= 1.0f * elapsedTime;
	}
}