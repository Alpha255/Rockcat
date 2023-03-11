#include "ParallaxMapping.h"
#include "Camera.h"
#include "D3DEngine.h"
#include "ImGUI.h"
#include "D3DLighting.h"

/// https://cgcookie.com/2017/06/15/normal-vs-displacement-mapping-why-games-use-normals/
/// https://msdn.microsoft.com/en-us/library/windows/desktop/ff476340(v=vs.85).aspx

struct ConstantBufferVS
{
	Matrix World;
	Matrix WorldInverse;
	Matrix WVP;
	Matrix VP;
	Matrix UVTransform;

	Vec4 EyePos;
	Vec4 LightDir;

	float MinTessFactor = 1.0f;
	float MaxTessFactor = 5.0f;
	float MinTessDistance = 1.0f;
	float MaxTessDistance = 25.0f;
};

struct ConstantBufferPS
{
	Vec3 EyePos;
	float HeightScale = 0.01f;

	DirectionalLight DirLight;
	Material::RawMaterial RawMat;

	ConstantBufferPS()
	{
		DirLight.Diffuse = Vec4(1.0f, 1.0f, 1.0f, 0.0f);
		DirLight.Direction = Vec4(1.0f, 1.0f, 5.0f, 0.0f);
		DirLight.SpecularIntensity = 64.0f;
	}
};

ConstantBufferVS g_CBufferVS;
ConstantBufferPS g_CBufferPS;

void AppParallaxMapping::Initialize()
{
	m_Floor.CreateAsQuad(8.0f);

	///m_HeightMap.Create("StoneFloor_Heightmap.dds");
	m_HeightMap.Create("bricks2_disp.dds");

	m_FloorMaterial.Set(Material::eDiffuse, "bricks2.dds");
	m_FloorMaterial.Set(Material::eSpecular, Vec4(1.0f, 1.0f, 1.0f, 0.0f));
	m_FloorMaterial.Set(Material::eNormal, "bricks2_normal.dds");
	///m_FloorMaterial.Set(Material::eDiffuse, "StoneFloor_Diffuse.dds");
	///m_FloorMaterial.Set(Material::eSpecular, "StoneFloor_Specular.dds");
	///m_FloorMaterial.Set(Material::eNormal, "StoneFloor_Normal.dds");

	m_CBufferVS.CreateAsConstantBuffer(sizeof(ConstantBufferVS), D3DBuffer::eGpuReadCpuWrite);
	m_CBufferPS.CreateAsConstantBuffer(sizeof(ConstantBufferPS), D3DBuffer::eGpuReadCpuWrite);

	m_VertexShader.Create("ParallaxMapping.hlsl", "VSMain");
	m_VSDisplacementMapping.Create("ParallaxMapping.hlsl", "VSMain_DisplacementMapping");

	m_HullShader.Create("ParallaxMapping.hlsl", "HSMain");
	m_DomainShader.Create("ParallaxMapping.hlsl", "DSMain");

	m_PixelShader[eNormalMapping].Create("ParallaxMapping.hlsl", "PSMain_NormalMapping");
	m_PixelShader[ePOMLearningOpenGL].Create("ParallaxMapping.hlsl", "PSMain_POMLearningOpenGL");
	m_PixelShader[eParallaxOcclusionMappingInACL].Create("ParallaxMapping.hlsl", "PSMain_ParallaxOcclusionMappingInACL");
	m_PixelShader[eParallaxMappingWithOffsetLimit].Create("ParallaxMapping.hlsl", "PSMain_ParallaxMappingWithOffsetLimit");
	m_PixelShader[eParallaxOcclusionMapping].Create("ParallaxMapping.hlsl", "PSMain_ParallaxOcclusionMapping");
	m_PixelShader[eDisplacementMapping].Create("ParallaxMapping.hlsl", "PSMain_DisplacementMapping");
}

void AppParallaxMapping::RenderScene()
{
	D3DEngine::Instance().ResetDefaultRenderSurfaces();
	D3DEngine::Instance().SetViewport(D3DViewport(0.0f, 0.0f, (float)m_Width, (float)m_Height));

	D3DEngine::Instance().SetPixelShader(m_PixelShader[m_MappingType]);

	m_Floor.Bind(&m_FloorMaterial);
	D3DEngine::Instance().SetShaderResourceView(m_HeightMap, 2U, D3DShader::ePixelShader);

	Vec4 eyePos = m_Camera->GetEyePos();
	g_CBufferPS.EyePos = Vec3(eyePos.x, eyePos.y, eyePos.z);
	g_CBufferPS.RawMat = m_FloorMaterial.RawValue;
	m_CBufferPS.Update(&g_CBufferPS, sizeof(ConstantBufferPS));
	D3DEngine::Instance().SetConstantBuffer(m_CBufferPS, 0U, D3DShader::ePixelShader);

	g_CBufferVS.EyePos = eyePos;
	g_CBufferVS.World = Matrix::Transpose(m_Camera->GetWorldMatrix());
	g_CBufferVS.WorldInverse = Matrix::InverseTranspose(m_Camera->GetWorldMatrix());
	g_CBufferVS.WVP = Matrix::Transpose(m_Camera->GetWVPMatrix());
	g_CBufferVS.VP = Matrix::Transpose(m_Camera->GetViewMatrix() * m_Camera->GetProjMatrix());
	g_CBufferVS.UVTransform = Matrix::Transpose(Matrix::Scaling(1.0f));
	g_CBufferVS.LightDir = g_CBufferPS.DirLight.Direction;
	m_CBufferVS.Update(&g_CBufferVS, sizeof(ConstantBufferVS));
	D3DEngine::Instance().SetConstantBuffer(m_CBufferVS, 0U, D3DShader::eVertexShader);

	ePrimitiveTopology ePT = eTriangleList;
	if (eDisplacementMapping == m_MappingType)
	{
		ePT = eControlPointPatchList3;
		D3DEngine::Instance().SetVertexShader(m_VSDisplacementMapping);

		D3DEngine::Instance().SetHullShader(m_HullShader);
		D3DEngine::Instance().SetDomainShader(m_DomainShader);

		D3DEngine::Instance().SetConstantBuffer(m_CBufferVS, 0U, D3DShader::eDomainShader);
		D3DEngine::Instance().SetSamplerState(D3DStaticState::LinearSampler, 0U, D3DShader::eDomainShader);
		D3DEngine::Instance().SetShaderResourceView(m_HeightMap, 0U, D3DShader::eDomainShader);
	}
	else
	{
		ePT = eTriangleList;
		D3DEngine::Instance().SetVertexShader(m_VertexShader);
	}

	if (m_Wireframe)
	{
		D3DEngine::Instance().SetRasterizerState(D3DStaticState::WireframeNoneCulling);
	}
	else
	{
		D3DEngine::Instance().SetRasterizerState(D3DStaticState::SolidNoneCulling);
	}

	D3DEngine::Instance().DrawIndexed(m_Floor.IndexCount, 0U, 0, ePT);

	///Vec3 pos(-g_CBufferPS.DirLight.Direction.x, -g_CBufferPS.DirLight.Direction.y, -g_CBufferPS.DirLight.Direction.z);
	///Light::DebugDisplay(pos, Light::ePoint, *m_Camera);

	static D3DHullShader EmptyHullShader;
	static D3DDomainShader EmptyDomainShader;
	D3DEngine::Instance().SetHullShader(EmptyHullShader);
	D3DEngine::Instance().SetDomainShader(EmptyDomainShader);

	if (m_MappingType > eNormalMapping)
	{
		ImGui::SliderFloat("HeightScale", &g_CBufferPS.HeightScale, 0.0f, 0.2f);
	}

	///D3DEngine::Instance().DrawTextInPos("RenderTextTest", 0U, 0U, 1U);

	ImGui::Text("%.2f FPS", m_FPS);
	ImGui::Combo("MappingType", &m_MappingType, 
		"NormalMapping\0"
		"POMLearningOpenGL\0"
		"ParallaxOcclusionMappingInACL\0"
		"ParallaxMappingWithOffsetLimit\0"
		"ParallaxOcclusionMapping\0"
		"DisplacementMapping");
	ImGui::Checkbox("Wireframe", &m_Wireframe);
	///ImGui::SliderFloat3("LightDir", (float *)&g_CBufferPS.DirLight.Direction, -10.0f, 10.0f);
}
