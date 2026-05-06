#include "RHI/RHISwapchain.h"
#include "RHI/RHIDevice.h"
#include "Application/Window.h"
#include "Application/BaseApplication.h"

RHIViewportClient::RHIViewportClient(const Window& InWindow, const RenderSettings& InRenderSettings)
	: m_Window(InWindow)
	, m_IsStereo(InRenderSettings.StereoRendering)
{
	RHISwapchainDesc Desc;
	Desc.SetWidth(InWindow.GetWidth())
		.SetHeight(InWindow.GetHeight())
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
