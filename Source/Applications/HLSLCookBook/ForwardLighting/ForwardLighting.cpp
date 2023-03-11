#include "ForwardLighting.h"
#include "Colorful/Public/ImGUI.h"
#include "Colorful/Public/Lighting.h"

struct ConstantBufferVS
{
	Matrix WVP;
	Matrix World;
	Matrix WorldInverse;
};

struct ConstantBufferPS
{
	Vec4 AmbientLowerClr;
	Vec4 AmbientRange;

	Vec4 EyePos;

	Material::RawMaterial RawMat;
	DirectionalLight DirLight;
	PointLight PLights[3];
	SpotLight SLights[3];
	CapsuleLight CLights[3];

	ConstantBufferPS()
	{
		DirLight.Direction = Vec4(1.0, -1.0f, 1.0f, 0.0f);
		DirLight.Diffuse = Vec4(0.85f, 0.8f, 0.5f, 1.0f);
		DirLight.Diffuse = DirLight.Diffuse * DirLight.Diffuse;

		PLights[0].Position = Vec3(25.0f, 13.0f, 14.4f);
		PLights[1].Position = Vec3(-25.0f, 13.0f, 14.4f);
		PLights[2].Position = Vec3(0.0f, 13.0f, -28.9f);
			  
		PLights[0].Diffuse = Vec4(1.0f, 0.0f, 0.0f, 1.0f);
		PLights[1].Diffuse = Vec4(0.0f, 1.0f, 0.0f, 1.0f);
		PLights[2].Diffuse = Vec4(0.0f, 0.0f, 1.0f, 1.0f);
			  
		PLights[0].Range = 50.0f;
		PLights[1].Range = 50.0f;
		PLights[2].Range = 50.0f;

		SLights[0].Position = Vec3(25.0f, 13.0f, 14.4f);
		SLights[1].Position = Vec3(-25.0f, 13.0f, 14.4f);
		SLights[2].Position = Vec3(0.0f, 13.0f, -28.9f);

		SLights[0].Diffuse = Vec4(1.0f, 0.0f, 0.0f, 1.0f);
		SLights[1].Diffuse = Vec4(0.0f, 1.0f, 0.0f, 1.0f);
		SLights[2].Diffuse = Vec4(0.0f, 0.0f, 1.0f, 1.0f);

		SLights[0].LookAt = Vec3(0.0f, 10.0f, 0.0f);
		SLights[1].LookAt = Vec3(0.0f, 10.0f, 0.0f);
		SLights[2].LookAt = Vec3(0.0f, 10.0f, 0.0f);

		CLights[0].Position = Vec3(25.0f, 13.0f, 14.4f);
		CLights[1].Position = Vec3(-25.0f, 13.0f, 14.4f);
		CLights[2].Position = Vec3(0.0f, 13.0f, -28.9f);
		
		CLights[0].Diffuse = Vec4(1.0f, 0.0f, 0.0f, 1.0f);
		CLights[1].Diffuse = Vec4(0.0f, 1.0f, 0.0f, 1.0f);
		CLights[2].Diffuse = Vec4(0.0f, 0.0f, 1.0f, 1.0f);

		CLights[0].Direction = CLights[2].Position - CLights[1].Position;
		CLights[1].Direction = CLights[0].Position - CLights[2].Position;
		CLights[2].Direction = CLights[1].Position - CLights[0].Position;
	}
};

Vec4 g_AmbientLowerClr = Vec4(0.1f, 0.5f, 0.1f, 1.0f);
Vec4 g_AmbientUpperClr = Vec4(0.1f, 0.2f, 0.5f, 1.0f);
ConstantBufferPS g_CBufferPS;
ConstantBufferVS g_CBufferVS;

void ForwardLighting::PrepareScene()
{
	m_Bunny.CreateFromFile("bunny.sdkmesh");

	AutoFocus(m_Bunny, 0.7f);

	m_VertexShader.Create("ForwardLighting.shader", "VSMain");
	m_PixelShader[eHemisphericAmbient].Create("ForwardLighting.shader", "PSMain_HemisphericAmbient");
	m_PixelShader[eDirectional].Create("ForwardLighting.shader", "PSMain_Directional");
	m_PixelShader[ePoint].Create("ForwardLighting.shader", "PSMain_Point");
	m_PixelShader[eSpot].Create("ForwardLighting.shader", "PSMain_Spot");
	m_PixelShader[eCapsule].Create("ForwardLighting.shader", "PSMain_Capsule");

	m_CBufferVS.CreateAsUniformBuffer(sizeof(ConstantBufferVS), eGpuReadCpuWrite);
	m_CBufferPS.CreateAsUniformBuffer(sizeof(ConstantBufferPS), eGpuReadCpuWrite);

	m_BunnyMaterial.Set(Material::eDiffuse, Vec4(0.85f, 0.8f, 0.5f, 1.0f));
	m_BunnyMaterial.Set(Material::eSpecular, Vec4(0.25f, 0.25f, 0.25f, 1.0f));
}

void ForwardLighting::RenderScene()
{
	REngine::Instance().ResetDefaultRenderSurfaces();
	REngine::Instance().SetViewport(RViewport(0.0f, 0.0f, (float)m_WindowSize.first, (float)m_WindowSize.second));

	REngine::Instance().SetVertexShader(m_VertexShader);
	REngine::Instance().SetPixelShader(m_PixelShader[m_LightingType]);

	REngine::Instance().SetUniformBuffer(m_CBufferVS, 0U, eVertexShader);
	REngine::Instance().SetUniformBuffer(m_CBufferPS, 0U, ePixelShader);

	Matrix world = Matrix::IdentityMatrix();
	g_CBufferVS.WVP = Matrix::Transpose(m_Camera.GetWVPMatrix());
	g_CBufferVS.World = Matrix::Transpose(world);
	g_CBufferVS.WorldInverse = Matrix::InverseTranspose(world);
	m_CBufferVS.Update(&g_CBufferVS, sizeof(ConstantBufferVS));

	g_CBufferPS.AmbientLowerClr = GammaToLinear(g_AmbientLowerClr);
	g_CBufferPS.AmbientRange = GammaToLinear(g_AmbientUpperClr) - GammaToLinear(g_AmbientLowerClr);
	Vec3 eyePos = m_Camera.GetEyePos();
	g_CBufferPS.EyePos = Vec4(eyePos.x, eyePos.y, eyePos.z, 0.0f);
	g_CBufferPS.RawMat = m_BunnyMaterial.RawValue;
	m_CBufferPS.Update(&g_CBufferPS, sizeof(ConstantBufferPS));

	if (m_Wireframe)
	{
		REngine::Instance().SetRasterizerState(RStaticState::Wireframe);
	}
	else
	{
		REngine::Instance().SetRasterizerState(RStaticState::Solid);
	}

	m_Bunny.Draw(m_Camera, m_DrawBoundingBox);

	if (eHemisphericAmbient == m_LightingType)
	{
		g_AmbientLowerClr = Vec4(0.1f, 0.5f, 0.1f, 1.0f);
		g_AmbientUpperClr = Vec4(0.1f, 0.2f, 0.5f, 1.0f);
	}
	else
	{
		g_AmbientLowerClr = Vec4(0.1f, 0.2f, 0.1f, 1.0f);
		g_AmbientUpperClr = Vec4(0.1f, 0.2f, 0.2f, 1.0f);
	}
	if (eDirectional == m_LightingType)
	{
		ImGui::SliderFloat3("DirtionalClr", (float *)&g_CBufferPS.DirLight.Diffuse, 0.0f, 1.0f);
	}
	if (eCapsule == m_LightingType)
	{
		static float s_CLightLength = 85.0f;
		static float s_CLightRange = 20.0f;
		ImGui::SliderFloat("CapsuleLightRange", &s_CLightRange, 10.0f, 50.0f);
		ImGui::SliderFloat("CapsuleLightLength", &s_CLightLength, 10.0f, 100.0f);
		for (uint32_t i = 0U; i < 3U; ++i)
		{
			g_CBufferPS.CLights[i].Length = s_CLightLength;
			g_CBufferPS.CLights[i].Range = s_CLightRange;
		}
	}

	ImGui::Combo("LightingType", &m_LightingType, "HemisphericAmbient\0DirectionalLight\0PointLight\0Spot\0Capsule");
	ImGui::Checkbox("Wireframe", &m_Wireframe);
	ImGui::Checkbox("BoundingBox", &m_DrawBoundingBox);
	ImGui::Text("FPS=%.2f", m_FPS);
}
