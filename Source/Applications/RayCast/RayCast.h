
#pragma once

#include "RenderApp.h"
#include "D3DGeometry.h"
#include "D3DView.h"

class AppRayCast : public RenderApp
{
public:
	AppRayCast() = default;
	~AppRayCast() = default;

	virtual void Initialize() override;
	virtual void RenderScene() override;
	virtual	void MouseButtonDown(::WPARAM, int32_t x, int32_t y) override;
protected:
	struct AxisAlignedBox
	{
		Vec3 Center;   /// Center of the box.
		Vec3 Extents;  /// Distance from the center to each side.
	};

	struct Vertex
	{
		Vec3 Pos;
		Vec3 Normal;
		Vec2 UV;
	};

	struct ConstantsBuffer
	{
		Matrix WVP;
		Vec4 Color;
	};

	void Pick(int32_t x, int32_t y);
	void LoadMesh();
	bool IsIntersectRayAxisAlignedBox(const Math::XMVECTOR &vOrigin, const Math::XMVECTOR &vDir, float &min);
	bool IsIntersectRayTriangle(const Math::XMVECTOR &vOrigin, const Math::XMVECTOR &vDir, const Vec3 &v0, const Vec3 &v1, const Vec3 &v2, float &min);
private:
	D3DVertexShader m_VertexShader;
	D3DPixelShader m_PixelShader;

	D3DBuffer m_VertexBuffer;
	D3DBuffer m_IndexBuffer;
	D3DBuffer m_ConstantBuffer;

	D3DInputLayout m_VertexLayout;

	std::vector<Vertex> m_Verties;
	std::vector<uint32_t> m_Indices;
	AxisAlignedBox m_MeshBox;

	uint32_t m_PickedTriangleIndex = UINT_MAX;
};
