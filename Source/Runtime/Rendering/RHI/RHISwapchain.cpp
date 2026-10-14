#include "Rendering/RHI/RHISwapchain.h"
#include "Rendering/RHI/RHIDevice.h"
#include "Application/Window.h"
#include "Application/BaseApplication.h"

#include <cmath>

namespace
{
	/// Logical units -> device pixels, rounding so a fractional scale (175%, 225%, ...) can never
	/// leave the swap chain a pixel short of the surface it has to fill.
	uint32_t ToDevicePixels(uint32_t LogicalValue, float DpiScale)
	{
		return static_cast<uint32_t>(std::lroundf(static_cast<float>(LogicalValue) * DpiScale));
	}
}

RHIViewportClient::RHIViewportClient(const Window& InWindow, const RenderSettings& InRenderSettings)
	: m_Window(InWindow)
	, m_IsStereo(InRenderSettings.StereoRendering)
{
	/// The client area is reported in logical units while the swap chain is measured in real
	/// device pixels. Skipping the conversion is what makes a UI on a HiDPI display look soft:
	/// everything is rendered small and afterwards stretched by the compositor.
	const float DpiScale = InWindow.GetDpiScale();

	RHISwapchainDesc Desc;
	Desc.SetWidth(ToDevicePixels(InWindow.GetWidth(), DpiScale))
		.SetHeight(ToDevicePixels(InWindow.GetHeight(), DpiScale))
		.SetFullscreen(InWindow.IsFullscreen())
		.SetVSync(InRenderSettings.VSync)
		.SetHDR(InRenderSettings.HDR)
		.SetWindowHandle(InWindow.GetHandle());

	extern BaseApplication* GApplication;
	m_Swapchain = GApplication->GetRenderDevice().CreateSwapchain(Desc);

	if (m_FinalOutputMode == EFinalOutputMode::Offscreen)
	{
		RHITextureDesc Desc;
		Desc.SetWidth(m_Window.GetWidth())
			.SetHeight(m_Window.GetHeight())
			.SetDimension(ERHITextureDimension::T_2D)
			.SetFormat(InRenderSettings.HDR ? ERHIFormat::RGB10A2_UNorm : ERHIFormat::RGBA8_UNorm)
			.SetUsages(ERHIBufferUsageFlags::RenderTarget | ERHIBufferUsageFlags::ShaderResource)
			.SetPermanentState(ERHIResourceState::RenderTarget);

		m_OffscreenViewSurface = GApplication->GetRenderDevice().CreateTexture(Desc);
	}
}

Math::UInt2 RHIViewportClient::GetViewSize() const
{
	return Math::UInt2(m_Window.GetWidth(), m_Window.GetHeight());
}

RHITexture* RHIViewportClient::GetViewSurface() const
{
	return m_FinalOutputMode == EFinalOutputMode::Default ? m_Swapchain->GetBackBuffer() : m_OffscreenViewSurface.get();
}
