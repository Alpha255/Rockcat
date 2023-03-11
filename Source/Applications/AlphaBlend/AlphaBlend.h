#pragma once

#include "RenderApp.h"
#include "D3DGeometry.h"
#include "D3DView.h"
#include "D3DState.h"

class AppAlphaBlend : public RenderApp
{
public:
	AppAlphaBlend() = default;
	~AppAlphaBlend() = default;

	virtual void Initialize() override;
	virtual void RenderScene() override;
protected:
private:
	Geometry::Mesh m_FloorMesh;
	Geometry::Mesh m_CubeMesh;
	Geometry::Mesh m_WindowMesh;

	D3DVertexShader m_VertexShader;
	D3DPixelShader m_PixelShader;

	D3DBuffer m_CBufferVS;

	D3DShaderResourceView m_TexFloor;
	D3DShaderResourceView m_TexCube;
	D3DShaderResourceView m_TexWindow;

	D3DBlendState m_AlphaBlend;

	bool m_bAlphaBlend = true;
};
