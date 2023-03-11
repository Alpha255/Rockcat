#pragma once

#include "RenderApp.h"
#include "D3DGeometry.h"
#include "D3DState.h"
#include "D3DView.h"

class AppDepthStencilTest : public RenderApp
{
public:
	AppDepthStencilTest() = default;
	~AppDepthStencilTest() = default;

	virtual void Initialize() override;
	virtual void RenderScene() override;
	virtual void Update(float elapsedTime, float totalTime) override;
protected:
	void UpdateConstantBufferVS(const Matrix &world);
	void DrawFloorAndWalls(const Matrix &world);
	void DrawSphere(const Matrix &world);
	void DrawMirror(const Matrix &world);
private:
	D3DShaderResourceView m_FloorTex;
	D3DShaderResourceView m_WallTex;
	D3DShaderResourceView m_MirrorTex;
	D3DShaderResourceView m_SphereTex;

	D3DVertexShader m_VertexShader;
	D3DPixelShader m_PixelShader;

	D3DBlendState m_NoColorWrite;
	D3DBlendState m_AlphaBlend;

	D3DDepthStencilState m_MarkMirror;
	D3DDepthStencilState m_Reflection;

	D3DBuffer m_CBufferVS;
	D3DBuffer m_CBufferPS;

	Vec2 m_SphereTrans = {};

	Geometry::ObjMesh m_RoomMesh;
	Geometry::Mesh m_SphereMesh;
};

