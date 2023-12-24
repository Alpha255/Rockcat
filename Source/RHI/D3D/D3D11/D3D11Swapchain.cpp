#include "RHI/D3D/D3D11/D3D11Swapchain.h"
#include "RHI/D3D/D3D11/D3D11Device.h"

D3D11SwapChain::D3D11SwapChain(
	const DxgiFactory& Factory,
	const D3D12Device& Device,
	uint64_t WindowHandle,
	uint32_t Width,
	uint32_t Height,
	bool8_t Fullscreen,
	bool8_t VSync,
	bool8_t sRGB)
	: m_FullScreen(Fullscreen)
	, m_VSync(VSync)
{
	assert(WindowHandle);
	/// Microsoft DirectX Graphics Infrastructure (DXGI)
	/// IDXGISwapChain1: Provides presentation capabilities that are enhanced from IDXGISwapChain. 
	/// These presentation capabilities consist of specifying dirty rectangles and scroll rectangle to optimize the presentation.
	/// IDXGISwapChain2: Extends IDXGISwapChain1 with methods to support swap back buffer scaling and lower-latency swap chains.
	/// IDXGISwapChain3: Extends IDXGISwapChain2 with methods to support getting the index of the swap chain's current back buffer and support for color space.

	DXGI_FORMAT Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	DXGI_SWAP_CHAIN_FULLSCREEN_DESC FullscreenDesc{};

	DXGI_SWAP_CHAIN_DESC1 Desc
	{
		Width,
		Height,
		Format,
		false,
		{
			1u,
			0u,
		},
		DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_BACK_BUFFER,
		3u,
		DXGI_SCALING_NONE,
		DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL,
		DXGI_ALPHA_MODE_UNSPECIFIED,
		DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH
	};

#if 0
	VERIFY_D3D(Factory->CreateSwapChainForHwnd(
		device,
		reinterpret_cast<::HWND>(windowHandle),
		&desc,
		&fullscreenDesc,
		nullptr,
		reference()));
#endif
}