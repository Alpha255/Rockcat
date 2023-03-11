#pragma once

#include "Public/IRenderApplication.h"
#include "Colorful/Public/Model.h"
#include "D3DGeometryBuffer.h"
#include "LightController.h"

class DeferredShading : public IRenderApplication
{
public:
	DeferredShading() = default;
	~DeferredShading() = default;

	void PrepareScene() override;
	void RenderScene() override;
	void ResizeWindow(uint32_t width, uint32_t height) override;
protected:
	struct ConstantBufferVS
	{
		Matrix World;
		Matrix WVP;
	};

	struct ConstantBufferPS
	{
		Vec4 LightSpecular;
	};
private:
	D3DGeometryBuffer m_GBuffer;
	LightController m_LightController;

	RVertexShader m_VertexShader;
	RPixelShader m_PixelShader;

	RBuffer m_CBufferVS;
	RBuffer m_CBufferPS;

	Geometry::Model m_Bunny;

	int32_t m_LightingType = eDirectionalLight;
	bool m_bVisualizeGBuffer = true;
};
