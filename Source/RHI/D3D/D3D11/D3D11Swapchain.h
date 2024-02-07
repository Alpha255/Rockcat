#pragma once

#include "RHI/D3D/DXGIInterface.h"

class D3D11SwapChain final : public D3DHwResource<IDXGISwapChain>
{
public:
	D3D11SwapChain(
		const DxgiFactory& Factory,
		const class D3D12Device& Device,
		uint64_t WindowHandle,
		uint32_t Width,
		uint32_t Height,
		bool Fullscreen,
		bool VSync,
		bool sRGB);
protected:
private:
	bool m_FullScreen = false;
	bool m_VSync = false;
};
