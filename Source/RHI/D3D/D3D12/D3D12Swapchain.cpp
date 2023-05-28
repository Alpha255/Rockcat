#include "Colorful/D3D/D3D12/D3D12Swapchain.h"
#include "Colorful/D3D/D3D12/D3D12Device.h"

#if 0
void D3D12Swapchain::Resize(uint32_t Width, uint32_t Height)
{
	VERIFY_D3D12(m_Object->ResizeBuffers(
		m_bTripleBuffer ? 3u : 2u,
		width, 
		height,
		DXGI_FORMAT_R8G8B8A8_UNORM,
		DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH
	));
}

#endif

NAMESPACE_START(RHI)

D3D12Swapchain::D3D12Swapchain(
	DxgiFactory* Factory,
	D3D12Device* Device,
	uint64_t WindowHandle, 
	uint32_t Width, 
	uint32_t Height, 
	bool8_t Fullscreen, 
	bool8_t VSync, 
	bool8_t SRgb)
	: D3DHWObject(WindowHandle, Width, Height, Fullscreen, VSync)
	, m_ColorFormat(SRgb ? DXGI_FORMAT_R8G8B8A8_UNORM_SRGB : DXGI_FORMAT_R8G8B8A8_UNORM)
{
	/// Microsoft DirectX Graphics Infrastructure (DXGI)
	/// IDXGISwapChain1: Provides presentation capabilities that are enhanced from IDXGISwapChain. 
	/// These presentation capabilities consist of specifying dirty rectangles and scroll rectangle to optimize the presentation.

	/// IDXGISwapChain2: Extends IDXGISwapChain1 with methods to support swap back buffer scaling and lower-latency swap chains.

	/// IDXGISwapChain3: Extends IDXGISwapChain2 with methods to support getting the index of the swap chain's current back buffer and support for color space.
	assert(Factory && Device);

	bool8_t AllowTearing = false;
	uint32_t Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	DxgiFactory5 Factory5;
	if (SUCCEEDED(Factory->Get()->QueryInterface(Factory5.Reference())))
	{
		Factory5->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &AllowTearing, sizeof(bool8_t));
		if (AllowTearing)
		{
			Flags |= DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;
		}
	}

	/****************************************************************************
		DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED: Scanline order is unspecified.
		DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE: The image is created from the first scanline to the last without skipping any.
		DXGI_MODE_SCANLINE_ORDER_UPPER_FIELD_FIRST: The image is created beginning with the upper field.
		DXGI_MODE_SCANLINE_ORDER_LOWER_FIELD_FIRST: The image is created beginning with the lower field.

		DXGI_MODE_SCALING_UNSPECIFIED: Unspecified scaling.
		DXGI_MODE_SCALING_CENTERED: Specifies no scaling. The image is centered on the display. This flag is typically used for a fixed-dot-pitch display (such as an LED display).
		DXGI_MODE_SCALING_STRETCHED: Specifies stretched scaling.
	******************************************************************************/
	const uint32_t DefaultBackBufferCount = 3u;

	DxgiFactory2 Factory2;
	if (SUCCEEDED(Factory->Get()->QueryInterface(Factory2.Reference())))
	{
		DXGI_SWAP_CHAIN_DESC1 Desc1
		{
			Width,
			Height,
			m_ColorFormat,
			false,
			DXGI_SAMPLE_DESC
			{
				1u,
				0u
			},
			DXGI_USAGE_SHADER_INPUT | DXGI_USAGE_RENDER_TARGET_OUTPUT,
			DefaultBackBufferCount,
			DXGI_SCALING_STRETCH,
			DXGI_SWAP_EFFECT_FLIP_DISCARD,
			DXGI_ALPHA_MODE_UNSPECIFIED,
			Flags
		};

		DXGI_SWAP_CHAIN_FULLSCREEN_DESC FullscreenDesc
		{
			DXGI_RATIONAL
			{
				0u,
				1u
			},
			DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE,
			DXGI_MODE_SCALING_UNSPECIFIED,
			!Fullscreen
		};

		VERIFY_D3D(Factory2->CreateSwapChainForHwnd(
			Device->CommandQueue(EQueueType::Graphics)->Get(),
			reinterpret_cast<HWND>(WindowHandle),
			&Desc1,
			&FullscreenDesc,
			nullptr,
			m_Swapchain1.Reference()));

		if (FAILED(m_Swapchain1->QueryInterface(m_Swapchain4.Reference())))
		{
			if (FAILED(m_Swapchain1->QueryInterface(m_Swapchain3.Reference())))
			{
				m_Swapchain1->QueryInterface(m_Swapchain2.Reference());
			}
		}
	}
	else
	{
		DXGI_SWAP_CHAIN_DESC Desc
		{
			DXGI_MODE_DESC
			{
				Width,
				Height,
				DXGI_RATIONAL
				{
					0u,
					1u
				},
				m_ColorFormat,
				DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE,
				DXGI_MODE_SCALING_UNSPECIFIED
			},
			DXGI_SAMPLE_DESC
			{
				1u,
				0u
			},
			DXGI_USAGE_SHADER_INPUT | DXGI_USAGE_RENDER_TARGET_OUTPUT,
			DefaultBackBufferCount,
			reinterpret_cast<HWND>(WindowHandle),
			!Fullscreen,
			DXGI_SWAP_EFFECT_FLIP_DISCARD,
			Flags
		};

		VERIFY_D3D(Factory->Get()->CreateSwapChain(
			Device->CommandQueue(EQueueType::Graphics)->Get(),
			&Desc,
			Reference()));

		if (FAILED(Get()->QueryInterface(m_Swapchain4.Reference())))
		{
			if (FAILED(Get()->QueryInterface(m_Swapchain3.Reference())))
			{
				if (FAILED(Get()->QueryInterface(m_Swapchain2.Reference())))
				{
					Get()->QueryInterface(m_Swapchain1.Reference());
				}
			}
		}
	}
}

D3D12Swapchain::~D3D12Swapchain()
{
}

NAMESPACE_END(RHI)