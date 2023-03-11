#pragma once

#include "Colorful/Public/Lighting.h"

class LightController
{
public:
	LightController() = default;
	~LightController() = default;

	void Init();

	inline void TurnonTheLights(eLightType lightType, const class D3DGeometryBuffer &GBuffer, const Vec4 &camPerspective, const Matrix &camViewMatrix)
	{
		assert(m_Inited && lightType < eLightTypeCount);
		
		switch (lightType)
		{
		case ePointLight: PointLighting(GBuffer, camPerspective, camViewMatrix); break;
		case eDirectionalLight: DirectionalLighting(GBuffer, camPerspective, camViewMatrix); break;
		case eSpotLight: SpotLighting(GBuffer, camPerspective, camViewMatrix); break;
		case eCapsuleLight: CapsuleLighting(GBuffer, camPerspective, camViewMatrix); break;
		}
	}

	Vec4 GetLightSpecular(eLightType type) const;

protected:
	void PointLighting(const class D3DGeometryBuffer &GBuffer, const Vec4 &camPerspective, const Matrix &camViewMatrix);
	void DirectionalLighting(const class D3DGeometryBuffer &GBuffer, const Vec4 &camPerspective, const Matrix &camViewMatrix);
	void SpotLighting(const class D3DGeometryBuffer &GBuffer, const Vec4 &camPerspective, const Matrix &camViewMatrix);
	void CapsuleLighting(const class D3DGeometryBuffer &GBuffer, const Vec4 &camPerspective, const Matrix &camViewMatrix);

	struct ConstantBufferPS
	{
		Vec4 AmbientLowerClr;
		Vec4 AmbientRange;

		Vec4 PerspectiveValue;

		Matrix ViewMatirxInvese;

		DirectionalLight DirLight;

		PointLight PointLights[3];
	};
private:
	std::array<PointLight, 3U> m_PointLights;
	SpotLight m_SpotLight;
	std::array<CapsuleLight, 3U> m_CapsuleLights;

	RVertexShader m_VertexShader;
	RPixelShader m_PixelShader[eLightTypeCount];
	RBuffer m_CBufferPS;
	ConstantBufferPS m_CBufferPSLighting;
	RDepthStencilState m_NoDepthWriteLessStencilMask;

	bool m_Inited = false;
};
