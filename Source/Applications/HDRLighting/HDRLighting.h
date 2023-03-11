#pragma once

#include "RenderApp.h"
#include "D3DGeometry.h"
#include "D3DView.h"

class AppHDRLighting : public RenderApp
{
public:
	AppHDRLighting() = default;
	~AppHDRLighting() = default;

	virtual void Initialize() override;
	virtual void RenderScene() override;
protected:
private:
	Geometry::Mesh m_Room;

	D3DVertexShader m_VertexShader;
	D3DPixelShader m_PixelShader;
	D3DBuffer m_CBufferVS;

	D3DShaderResourceView m_DiffuseTex;
};
