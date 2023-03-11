#pragma once

#include "Public/IRenderApplication.h"
#include "Public/Definitions.h"
#include "Public/Model.h"

class Box : public IRenderApplication
{
public:
	Box() = default;
	~Box() = default;

	void PrepareScene() override;
	void RenderScene() override;
protected:
	enum eSpecialEffect
	{
		eNone,
		eInversion,
		eGrayscale,
		eSharpen,
		eBlur,
		eEdgeDetection,
		eEffectCount
	};
private:
	Geometry::Model m_Box;
	RBuffer m_CBufferVS;
	RVertexShader m_VS;
	RPixelShader m_PS[eEffectCount];
	RShaderResourceView m_DiffuseTex;

	int32_t m_Effect = eNone;
	bool m_bVSync = false;
	bool m_bWireframe = false;
};
