#include "Cubemap.h"
#include "Camera.h"
#include "D3DLighting.h"
#include "D3DEngine.h"
#include "D3DTexture.h"

struct ConstantBufferVS
{
	Matrix World;
	Matrix WorldInverse;
	Matrix WVP;
};

struct ConstantBufferPS
{
	Vec4 EyePos;

	DirectionalLight Light;
	Material::RawMaterial Mat;

	ConstantBufferPS()
	{
		Light.Diffuse = Vec4(0.5f, 0.5f, 0.5f, 1.0f);
		Light.Specular = Vec3(0.5f, 0.5f, 0.5f);
		Light.Direction = Vec4(0.57735f, -0.57735f, 0.57735f, 0.0f);
	}
};

void AppCubemap::InitCubemapCameras(float cx, float cy, float cz)
{
	Vec3 worldCenter = Vec3(cx, cy, cz);

	Vec3 lookAts[6] =
	{
		Vec3(cx + 1.0f, cy, cz), /// +X
		Vec3(cx - 1.0f, cy, cz), /// -X
		Vec3(cx, cy + 1.0f, cz), /// +Y
		Vec3(cx, cy - 1.0f, cz), /// -Y
		Vec3(cx, cy, cz + 1.0f), /// +Z
		Vec3(cx, cy, cz - 1.0f), /// -Z
	};

	Vec3 upDirs[6] =
	{
		Vec3(0.0f, 1.0f,  0.0f), /// +X
		Vec3(0.0f, 1.0f,  0.0f), /// -X
		Vec3(0.0f, 0.0f, -1.0f), /// +Y
		Vec3(0.0f, 0.0f,  1.0f), /// -Y
		Vec3(0.0f, 1.0f,  0.0f), /// +Z
		Vec3(0.0f, 1.0f,  0.0f), /// -Z
	};

	for (uint32_t i = 0U; i < 6U; ++i)
	{
		m_CubemapCamera[i].SetViewParams(worldCenter, lookAts[i], upDirs[i]);
		m_CubemapCamera[i].SetProjParams(Math::XM_PIDIV2, 1.0f, 0.1f, 1000.0f);
	}
}

void AppCubemap::Initialize()
{
	m_SkyBox.Create("sunsetcube1024.dds", 5000.0f);

	m_CubeMesh.CreateAsCube(3.0f);
	m_DiffuseTexCube.Create("WoodCrate01.dds");
	m_DiffuseTexSphere.Create("wood.dds");

	m_SphereMesh.CreateAsGeoSphere(1.0f, 4U);

	m_VertexShader.Create("Cubemap.hlsl", "VSMain");
	m_PixelShader.Create("Cubemap.hlsl", "PSMain");

	m_CBufferVS.CreateAsConstantBuffer(sizeof(ConstantBufferVS), D3DBuffer::eGpuReadCpuWrite, nullptr);
	m_CBufferPS.CreateAsConstantBuffer(sizeof(ConstantBufferPS), D3DBuffer::eGpuReadCpuWrite, nullptr);

	InitCubemapCameras(0.0f, 2.0f, 0.0f);

	const uint32_t CubemapSize = 1024U;
	D3DTexture2D cubemapTex;
	cubemapTex.Create(eRGBA8_UNorm, CubemapSize, CubemapSize, D3DBuffer::eBindAsRenderTarget | D3DBuffer::eBindAsShaderResource, 
		0U, 6U, 0U, D3DBuffer::eMiscGenerateMips | D3DBuffer::eMiscCubemap);
	m_CubeMap.CreateAsTexture(D3DView::eTextureCube, cubemapTex, eRGBA8_UNorm, 0U, 1U);

	for (uint32_t i = 0U; i < 6U; ++i)
	{
		m_CubemapRTV[i].CreateAsTextureArray(D3DView::eTexture2DArray, cubemapTex, eRGBA8_UNorm, 0U, i, 1U);
	}

	m_CubemapDSV.Create(eD32_Float, CubemapSize, CubemapSize);

	m_CubemapViewport = { 0.0f, 0.0f, (float)CubemapSize, (float)CubemapSize, 0.0f, 1.0f };
	
	m_MatrialCube.Set(Material::eDiffuse, "WoodCrate01.dds");
	m_MatrialCube.Set(Material::eSpecular, Vec4(0.8f, 0.8f, 0.8f, 1.0f));

	m_MatrialSphere.Set(Material::eDiffuse, "stone.dds");
	m_MatrialSphere.Set(Material::eSpecular, Vec4(0.8f, 0.8f, 0.8f, 16.0f));
	m_MatrialSphere.Set(Material::eReflection, Vec4(0.8f, 0.8f, 0.8f, 1.0f));

	m_Camera->Translation(0.0f, 2.0f, -15.0f);
}

void AppCubemap::DrawClutter(const Camera &cam, bool isGenCubemap)
{
	ConstantBufferVS CBufferVS;
	ConstantBufferPS CBufferPS;

	D3DEngine::Instance().SetVertexShader(m_VertexShader);
	D3DEngine::Instance().SetPixelShader(m_PixelShader);

	D3DEngine::Instance().SetConstantBuffer(m_CBufferVS, 0U, D3DShader::eVertexShader);
	D3DEngine::Instance().SetConstantBuffer(m_CBufferPS, 0U, D3DShader::ePixelShader);

	/// Draw Cube
	Matrix translation = Matrix::Translation(0.0f, -2.0f, 10.0f);
	Matrix world = cam.GetWorldMatrix() * translation;
	CBufferVS.World = Matrix::Transpose(world);
	CBufferVS.WorldInverse = Matrix::Inverse(world);
	CBufferVS.WVP = Matrix::Transpose(world * cam.GetViewMatrix() * cam.GetProjMatrix());
	m_CBufferVS.Update(&CBufferVS, sizeof(ConstantBufferVS));

	CBufferPS.EyePos = cam.GetEyePos();
	CBufferPS.Mat = m_MatrialCube.RawValue;
	m_CBufferPS.Update(&CBufferPS, sizeof(ConstantBufferPS));

	D3DShaderResourceView NoneSRV;
	m_CubeMesh.Bind(&m_MatrialCube);
	D3DEngine::Instance().SetShaderResourceView(NoneSRV, 1U, D3DShader::ePixelShader);
	D3DEngine::Instance().DrawIndexed(m_CubeMesh.IndexCount, 0U, 0, eTriangleList);

	/// Draw Sphere
	if (!isGenCubemap)
	{
		translation = Matrix::Translation(0.0f, 1.0f, 10.0f);
		world = cam.GetWorldMatrix() * translation;
		CBufferVS.World = Matrix::Transpose(world);
		CBufferVS.WorldInverse = Matrix::Inverse(world);
		CBufferVS.WVP = Matrix::Transpose(world * cam.GetViewMatrix() * cam.GetProjMatrix());
		m_CBufferVS.Update(&CBufferVS, sizeof(ConstantBufferVS));

		CBufferPS.Mat = m_MatrialSphere.RawValue;
		m_CBufferPS.Update(&CBufferPS, sizeof(ConstantBufferPS));

		m_SphereMesh.Bind(&m_MatrialSphere);
		D3DEngine::Instance().SetShaderResourceView(m_CubeMap, 1U, D3DShader::ePixelShader);
		D3DEngine::Instance().DrawIndexed(m_SphereMesh.IndexCount, 0U, 0, eTriangleList);
	}

	/// Draw Sky Box
	m_SkyBox.Draw(cam);
}

void AppCubemap::RenderScene()
{
	/// Draw Dynamic Cubemap
	D3DEngine::Instance().SetViewport(m_CubemapViewport, 0U);
	D3DEngine::Instance().SetDepthStencilView(m_CubemapDSV);
	for (uint32_t i = 0U; i < 6U; ++i)
	{
		D3DEngine::Instance().SetRenderTargetView(m_CubemapRTV[i], 0U);

		D3DEngine::Instance().ClearRenderTargetView(m_CubemapRTV[i], Color::Silver);
		D3DEngine::Instance().ClearDepthStencilView(m_CubemapDSV, D3DDepthStencilView::eDepthStencil, 1.0f, 0U);

		DrawClutter(m_CubemapCamera[i], true);
	}

	/// Draw Scene
	D3DEngine::Instance().ResetDefaultRenderSurfaces();
	D3DEngine::Instance().SetViewport(D3DViewport(0.0f, 0.0f, (float)m_Width, (float)m_Height));
	DrawClutter(*m_Camera, false);

	m_SkyBox.Draw(*m_Camera);
}