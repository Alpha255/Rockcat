#pragma once

#include "RenderApp.h"
#include "D3DGeometry.h"
#include "D3DLighting.h"

class AppLighting : public RenderApp
{
public:
	enum eShadingMode
	{
		eFlat,
		eGouraud,
		ePhong,
		eBlinnPhong,
		eShadingModeCount
	};

	AppLighting() = default;
	~AppLighting() = default;

	virtual void Initialize() override;
	virtual void RenderScene() override;
protected:
private:
	Geometry::Mesh m_FlatSphere;
	Geometry::Mesh m_Sphere;

	D3DVertexShader m_VertexShader[eShadingModeCount];
	D3DPixelShader m_PixelShader[eShadingModeCount];

	D3DBuffer m_CBufferVS;
	D3DBuffer m_CBufferPS;

	Material m_SphereMaterial;

	eShadingMode m_ShadingMode = eFlat;
};
