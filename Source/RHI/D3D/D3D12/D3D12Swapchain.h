#pragma once

#include "RHI/D3D/DXGIInterface.h"

class D3D12Swapchain : public D3DHwResource<IDXGISwapChain>
{
public:
	D3D12Swapchain(
		const DxgiFactory& Factory,
		const class D3D12Device& Device,
		uint64_t WindowHandle,
		uint32_t Width,
		uint32_t Height,
		bool Fullscreen,
		bool VSync,
		bool sRGB);

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
