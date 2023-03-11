#pragma once

#include "Colorful/Public/Definitions.h"

class D3DGeometryBuffer
{
public:
	enum eBufferType
	{
		eDepthStencil,
		eColorSpecIntensity,
		eNormal,
		eSpecPower,
		eBufferTypeCount
	};

	D3DGeometryBuffer() = default;
	~D3DGeometryBuffer() = default;

	void Init(uint32_t width, uint32_t height, bool bReinit = false);

	void Resize(uint32_t width, uint32_t height);

	void Bind();

	void UnBind();

	void VisulizeGBuffer(bool bVisulize, const Vec4 &camPerspective);

	inline RShaderResourceView GetShaderResourceView(eBufferType type) const
	{
		assert(type < eBufferTypeCount);
		return m_ShaderResourceViews[type];
	}
protected:
	struct ConstantBufferPS
	{
		Vec4 Perspective;
	};
private:
	std::array<RShaderResourceView, eBufferTypeCount> m_ShaderResourceViews;

	RRenderTargetView m_SurfaceColorSpecIntensity;
	RRenderTargetView m_SurfaceNormal;
	RRenderTargetView m_SurfaceSpecPower;

	RDepthStencilView m_SurfaceDepthStencil;
	RDepthStencilView m_SurfaceDepthStencilReadonly;

	RDepthStencilState m_DepthStencilState;
	RBuffer m_CBufferPS;

	RVertexShader m_VertexShader;
	RPixelShader m_PixelShader;
};
