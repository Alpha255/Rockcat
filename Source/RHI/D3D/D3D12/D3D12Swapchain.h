#pragma once

#include "Colorful/IRenderer/IDevice.h"
#include "Colorful/D3D/DXGI_Interface.h"

NAMESPACE_START(RHI)

class D3D12Swapchain : public D3DHWObject<ISwapchain, IDXGISwapChain>
{
public:
	D3D12Swapchain(
		DxgiFactory* Factory,
		class D3D12Device* Device,
		uint64_t WindowHandle,
		uint32_t Width,
		uint32_t Height,
		bool8_t Fullscreen,
		bool8_t VSync,
		bool8_t SRgb);

	~D3D12Swapchain();
protected:
private:
	uint32_t m_CurImageIndex = 0u;
	DXGI_FORMAT m_ColorFormat = DXGI_FORMAT::DXGI_FORMAT_UNKNOWN;

	DxgiSwapChain1 m_Swapchain1;
	DxgiSwapChain2 m_Swapchain2;
	DxgiSwapChain3 m_Swapchain3;
	DxgiSwapChain4 m_Swapchain4;
};

NAMESPACE_END(RHI)
