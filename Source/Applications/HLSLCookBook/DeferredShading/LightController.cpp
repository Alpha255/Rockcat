#include "LightController.h"
#include "D3DGeometryBuffer.h"

void LightController::Init()
{
	m_NoDepthWriteLessStencilMask.Create(
		true, eDepthMaskZero, eRComparisonFunc::eLess,
		true, eStencilDefaultReadMask, eStencilDefaultWriteMask,
		eRStencilOp::eKeep, eRStencilOp::eKeep, eRStencilOp::eKeep, eRComparisonFunc::eEqual,
		eRStencilOp::eKeep, eRStencilOp::eKeep, eRStencilOp::eKeep, eRComparisonFunc::eEqual);

	m_VertexShader.Create("DeferredShading\\Lighting.hlsl", "VSMain");
	m_PixelShader[eDirectionalLight].Create("DeferredShading\\Lighting.hlsl", "PSMain");

	m_CBufferPS.CreateAsUniformBuffer(sizeof(ConstantBufferPS), eGpuReadCpuWrite);

	Vec4 ambientLowerClr = { 0.1f, 0.2f, 0.1f, 1.0f };
	Vec4 ambientUpperClr = { 0.1f, 0.2f, 0.2f, 1.0f };
	m_CBufferPSLighting.AmbientLowerClr = ambientLowerClr;
	m_CBufferPSLighting.AmbientRange = ambientUpperClr - ambientLowerClr;
	m_CBufferPSLighting.DirLight.Diffuse = Vec4(0.85f, 0.8f, 0.5f, 0.0f);
	m_CBufferPSLighting.DirLight.Direction = Vec4(1.0f, -1.0f, 1.0f, 0.0f);
	m_CBufferPSLighting.DirLight.Specular = Vec3(0.25f, 0.25f, 0.25f);
	m_CBufferPSLighting.DirLight.SpecularIntensity = 250.0f;

	m_CBufferPSLighting.PointLights[0].Position = Vec3(25.0f, 13.0f, 14.4f);
	m_CBufferPSLighting.PointLights[0].Diffuse = Vec4(1.25f, 0.0f, 0.0f, 1.0f);
	m_CBufferPSLighting.PointLights[0].Range = 25.0f;
	m_CBufferPSLighting.PointLights[0].Specular = Vec3(0.25f, 0.25f, 0.25f);
	m_CBufferPSLighting.PointLights[0].SpecularIntensity = 250.0f;

	m_CBufferPSLighting.PointLights[1].Position = Vec3(-25.0f, 13.0f, 14.4f);
	m_CBufferPSLighting.PointLights[1].Diffuse = Vec4(0.0f, 1.25f, 0.0f, 1.0f);
	m_CBufferPSLighting.PointLights[1].Range = 25.0f;
	m_CBufferPSLighting.PointLights[1].Specular = Vec3(0.25f, 0.25f, 0.25f);
	m_CBufferPSLighting.PointLights[1].SpecularIntensity = 250.0f;

	m_CBufferPSLighting.PointLights[2].Position = Vec3(0.0f, 13.0f, -28.9f);
	m_CBufferPSLighting.PointLights[2].Diffuse = Vec4(0.0f, 0.0f, 1.25f, 1.0f);
	m_CBufferPSLighting.PointLights[2].Range = 25.0f;
	m_CBufferPSLighting.PointLights[2].Specular = Vec3(0.25f, 0.25f, 0.25f);
	m_CBufferPSLighting.PointLights[2].SpecularIntensity = 250.0f;

	m_Inited = true;
}

Vec4 LightController::GetLightSpecular(eLightType type) const
{
	assert(m_Inited);

	Vec4 specular;
	switch (type)
	{
	case ePointLight:
		specular = Vec4(m_CBufferPSLighting.PointLights[0].Specular.x, m_CBufferPSLighting.PointLights[0].Specular.y,
			m_CBufferPSLighting.PointLights[0].Specular.z, m_CBufferPSLighting.PointLights[0].SpecularIntensity);
		break;
	case eDirectionalLight:
		specular = Vec4(m_CBufferPSLighting.DirLight.Specular.x, m_CBufferPSLighting.DirLight.Specular.y,
			m_CBufferPSLighting.DirLight.Specular.z, m_CBufferPSLighting.DirLight.SpecularIntensity);
		break;
	case eSpotLight:
		break;
	case eCapsuleLight:
		break;
	}

	return specular;
}

void LightController::PointLighting(const D3DGeometryBuffer &GBuffer, const Vec4 &camPerspective, const Matrix &camViewMatrix)
{
	/// Compared to the directional light, point lights may only cover parts of the screen.
	/// In addition, it may be partially or fully occluded by the scene as viewed when generating the GBuffer.
	/// Fortunately, all we need to represent the light source volume is to render the front of a
	/// sphere located at the point light's center position and scale to its range
}

void LightController::DirectionalLighting(const D3DGeometryBuffer &GBuffer, const Vec4 &camPerspective, const Matrix &camViewMatrix)
{
	REngine::Instance().SetDepthStencilState(m_NoDepthWriteLessStencilMask, 0x01U);

	REngine::Instance().SetVertexShader(m_VertexShader);
	REngine::Instance().SetPixelShader(m_PixelShader[eDirectionalLight]);

	RInputLayout EmptyLayout;
	REngine::Instance().SetInputLayout(EmptyLayout);

	RBuffer EmptyBuffer;
	REngine::Instance().SetVertexBuffer(EmptyBuffer, 0U, 0U, 0U);

	m_CBufferPSLighting.PerspectiveValue = camPerspective;
	m_CBufferPSLighting.ViewMatirxInvese = Matrix::InverseTranspose(camViewMatrix);
	m_CBufferPS.Update(&m_CBufferPSLighting, sizeof(ConstantBufferPS));
	REngine::Instance().SetUniformBuffer(m_CBufferPS, 0U, ePixelShader);

	for (uint32_t i = 0U; i < D3DGeometryBuffer::eBufferTypeCount; ++i)
	{
		REngine::Instance().SetShaderResourceView(GBuffer.GetShaderResourceView((D3DGeometryBuffer::eBufferType)i), i, ePixelShader);
	}

	REngine::Instance().SetSamplerState(RStaticState::LinearSampler, 0U, ePixelShader);

	REngine::Instance().Draw(4U, 0U, eTriangleStrip);
}

void LightController::SpotLighting(const D3DGeometryBuffer &GBuffer, const Vec4 &camPerspective, const Matrix &camViewMatrix)
{
}

void LightController::CapsuleLighting(const D3DGeometryBuffer &GBuffer, const Vec4 &camPerspective, const Matrix &camViewMatrix)
{
}
