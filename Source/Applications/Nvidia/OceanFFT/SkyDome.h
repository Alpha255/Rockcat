#pragma once

#include "D3DGraphic.h"

class SkyDome
{
public:
	SkyDome() = default;
	~SkyDome() = default;

	void Create(uint32_t width, uint32_t height);

	void Draw(const class Camera &cam);

	void Resize(uint32_t width, uint32_t height);
protected:
	void RecreateVertexBuffer(uint32_t width, uint32_t height);
private:
	struct D3DResource
	{
		Ref<ID3D11VertexShader> VertexShader;
		Ref<ID3D11PixelShader> PixelShader;

		Ref<ID3D11InputLayout> InputLayout;
		
		Ref<ID3D11Buffer> VertexBuffer;
		Ref<ID3D11Buffer> ConstantsBuffer;

		Ref<ID3D11ShaderResourceView> Cubemap;

		Ref<ID3D11SamplerState> LinearSampler;
		Ref<ID3D11DepthStencilState> DisableDepthStencil;
	}m_Res;

	bool m_bInited = false;
};
