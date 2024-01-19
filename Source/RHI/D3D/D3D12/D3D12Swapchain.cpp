#include "RHI/D3D/D3D12/D3D12Swapchain.h"
#include "RHI/D3D/D3D12/D3D12Device.h"
#include "Runtime/Engine/Services/SpdLogService.h"

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

D3D12Swapchain::D3D12Swapchain(
	const DxgiFactory& Factory,
	const D3D12Device& Device,
	uint64_t WindowHandle, 
	uint32_t Width, 
	uint32_t Height, 
	bool8_t Fullscreen, 
	bool8_t VSync, 
	bool8_t sRGB)
	: m_ColorFormat(sRGB ? DXGI_FORMAT_R8G8B8A8_UNORM_SRGB : DXGI_FORMAT_R8G8B8A8_UNORM)
{
	/// Microsoft DirectX Graphics Infrastructure (DXGI)
	/// IDXGISwapChain1: Provides presentation capabilities that are enhanced from IDXGISwapChain. 
	/// These presentation capabilities consist of specifying dirty rectangles and scroll rectangle to optimize the presentation.

	/// IDXGISwapChain2: Extends IDXGISwapChain1 with methods to support swap back buffer scaling and lower-latency swap chains.

	/// IDXGISwapChain3: Extends IDXGISwapChain2 with methods to support getting the index of the swap chain's current back buffer and support for color space.

	bool8_t AllowTearing = false;
	uint32_t Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	DxgiFactory5 Factory5;
	if (SUCCEEDED(Factory->QueryInterface(Factory5.Reference())))
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

	DXGI_SAMPLE_DESC SampleDesc
	{
		.Count = 1u,
		.Quality = 0u
	};

	DXGI_RATIONAL DXGIRational
	{
		.Numerator = 0u,
		.Denominator = 1u
	};

	DxgiFactory2 Factory2;
	if (SUCCEEDED(Factory->QueryInterface(Factory2.Reference())))
	{
		DXGI_SWAP_CHAIN_DESC1 Desc1
		{
			.Width = Width,
			.Height = Height,
			.Format = m_ColorFormat,
			.Stereo = false,
			.SampleDesc = SampleDesc,
			.BufferUsage = DXGI_USAGE_SHADER_INPUT | DXGI_USAGE_RENDER_TARGET_OUTPUT,
			.BufferCount = DefaultBackBufferCount,
			.Scaling = DXGI_SCALING_STRETCH,
			.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD,
			.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED,
			.Flags = Flags
		};

		DXGI_SWAP_CHAIN_FULLSCREEN_DESC FullscreenDesc
		{
			.RefreshRate = DXGIRational,
			.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE,
			.Scaling = DXGI_MODE_SCALING_UNSPECIFIED,
			.Windowed = !Fullscreen
		};

		VERIFY_D3D(Factory2->CreateSwapChainForHwnd(
			//Device->CommandQueue(EQueueType::Graphics)->Get(),
			nullptr,
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
		DXGI_MODE_DESC DXGIModeDesc
		{
			.Width = Width,
			.Height = Height,
			.RefreshRate = DXGIRational,
			.Format = m_ColorFormat,
			.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE,
			.Scaling = DXGI_MODE_SCALING_UNSPECIFIED
		};

		DXGI_SWAP_CHAIN_DESC Desc
		{
			.BufferDesc = DXGIModeDesc,
			.SampleDesc = SampleDesc,
			.BufferUsage = DXGI_USAGE_SHADER_INPUT | DXGI_USAGE_RENDER_TARGET_OUTPUT,
			.BufferCount = DefaultBackBufferCount,
			.OutputWindow = reinterpret_cast<HWND>(WindowHandle),
			.Windowed = !Fullscreen,
			.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD,
			.Flags = Flags
		};

		VERIFY_D3D(Factory->CreateSwapChain(
			//Device->CommandQueue(EQueueType::Graphics)->Get(),
			nullptr,
			&Desc,
			Reference()));

		if (FAILED(GetNative()->QueryInterface(m_Swapchain4.Reference())))
		{
			if (FAILED(GetNative()->QueryInterface(m_Swapchain3.Reference())))
			{
				if (FAILED(GetNative()->QueryInterface(m_Swapchain2.Reference())))
				{
					GetNative()->QueryInterface(m_Swapchain1.Reference());
				}
			}
		}
	}
}

D3D12Swapchain::~D3D12Swapchain()
{
}
