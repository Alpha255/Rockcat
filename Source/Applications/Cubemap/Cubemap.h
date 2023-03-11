#pragma once

#include "Camera.h"
#include "RenderApp.h"
#include "D3DSkyBox.h"
#include "D3DLighting.h"

class AppCubemap : public RenderApp
{
public:
	AppCubemap() = default;
	~AppCubemap() = default;

	virtual void Initialize() override;
	virtual void RenderScene() override;
protected:
	void InitCubemapCameras(float cx, float cy, float cz);

	void DrawClutter(const Camera &cam, bool isGenCubemap);
private:
	Geometry::Mesh m_CubeMesh;
	Geometry::Mesh m_SphereMesh;

	D3DSkyBox m_SkyBox;

	D3DVertexShader m_VertexShader;
	D3DPixelShader m_PixelShader;

	D3DBuffer m_CBufferVS;
	D3DBuffer m_CBufferPS;

	D3DShaderResourceView m_DiffuseTexCube;
	D3DShaderResourceView m_DiffuseTexSphere;
	D3DShaderResourceView m_CubeMap;

	D3DRenderTargetView m_CubemapRTV[6];
	D3DDepthStencilView m_CubemapDSV;

	Material m_MatrialCube;
	Material m_MatrialSphere;

	D3DViewport m_CubemapViewport;
	Camera m_CubemapCamera[6];
};
