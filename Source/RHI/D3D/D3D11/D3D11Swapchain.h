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
		bool8_t Fullscreen,
		bool8_t VSync,
		bool8_t sRGB);
protected:
private:
	bool8_t m_FullScreen = false;
	bool8_t m_VSync = false;
};
