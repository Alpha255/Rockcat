#include "Colorful/D3D/D3D11/D3D11Swapchain.h"

NAMESPACE_START(Gfx)

D3D11SwapChain::D3D11SwapChain(
	uint64_t windowHandle, 
	uint32_t width, 
	uint32_t height, 
	bool8_t fullscreen,
	bool8_t VSync,
	bool8_t tripleBuffer,
	IDXGIFactory2* factory,
	ID3D11Device* device)
	: m_FullScreen(fullscreen)
	, m_VSync(VSync)
{
	assert(windowHandle && factory && device);
	/// Microsoft DirectX Graphics Infrastructure (DXGI)
	/// IDXGISwapChain1: Provides presentation capabilities that are enhanced from IDXGISwapChain. 
	/// These presentation capabilities consist of specifying dirty rectangles and scroll rectangle to optimize the presentation.
	/// IDXGISwapChain2: Extends IDXGISwapChain1 with methods to support swap back buffer scaling and lower-latency swap chains.
	/// IDXGISwapChain3: Extends IDXGISwapChain2 with methods to support getting the index of the swap chain's current back buffer and support for color space.

	DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;
	DXGI_SWAP_CHAIN_FULLSCREEN_DESC fullscreenDesc{};

	DXGI_SWAP_CHAIN_DESC1 desc
	{
		m_FullScreen ? 1920 : width,
		m_FullScreen ? 1080 : height,
		format,
		false,
		{
			1u,
			0u,
		},
		DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_BACK_BUFFER,
		tripleBuffer ? 3u : 2u,
		DXGI_SCALING_NONE,
		DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL,
		DXGI_ALPHA_MODE_UNSPECIFIED,
		DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH
	};

	VERIFY_D3D(factory->CreateSwapChainForHwnd(
		device,
		reinterpret_cast<::HWND>(windowHandle),
		&desc,
		&fullscreenDesc,
		nullptr,
		reference()));
}

//void D3D11SwapChain::resize(uint32_t width, uint32_t height)
//{
//	GfxVerifyD3D(m_Object->ResizeBuffers(
//		m_bTripleBuffer ? 3u : 2u,
//		width, 
//		height,
//		DXGI_FORMAT_R8G8B8A8_UNORM,
//		DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH
//	));
//}

NAMESPACE_END(Gfx)