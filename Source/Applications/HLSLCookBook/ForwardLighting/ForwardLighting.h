#pragma once

#include "Public/IRenderApplication.h"
#include "Colorful/Public/Model.h"

class ForwardLighting : public IRenderApplication
{
public:
	ForwardLighting() = default;
	~ForwardLighting() = default;

	void PrepareScene() override;
	void RenderScene() override;
protected:
	inline Vec4 GammaToLinear(const Vec4 &color)
	{
		/// Approximation
		return color * color;
	}
private:
	enum eLightingType
	{
		eHemisphericAmbient,
		eDirectional,
		ePoint,
		eSpot,
		eCapsule,
		eCount
	};

	Geometry::Model m_Bunny;

	RVertexShader m_VertexShader;
	RPixelShader m_PixelShader[eCount];
	RBuffer m_CBufferVS;
	RBuffer m_CBufferPS;

	Material m_BunnyMaterial;

	int32_t m_LightingType = eHemisphericAmbient;
	bool m_Wireframe = false;
	bool m_DrawBoundingBox = false;
};
