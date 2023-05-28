#pragma once

#include "Colorful/D3D/DXGI_Interface.h"

NAMESPACE_START(Gfx)

class D3D11SwapChain final : public D3DObject<IDXGISwapChain1>
{
public:
	D3D11SwapChain(
		uint64_t windowHandle,
		uint32_t width,
		uint32_t height,
		bool8_t fullscreen,
		bool8_t VSync,
		bool8_t tripleBuffer,
		IDXGIFactory2* factory,
		ID3D11Device* device);

	inline void present()
	{
		VERIFY_D3D((*this)->Present(m_VSync ? 1u : 0u, 0u));
	}
protected:
private:
	DXGISwapChain2 m_SwapChain2;
	DXGISwapChain3 m_SwapChain3;
	DXGISwapChain4 m_SwapChain4;

	bool8_t m_FullScreen = false;
	bool8_t m_VSync = false;
};

NAMESPACE_END(Gfx)
