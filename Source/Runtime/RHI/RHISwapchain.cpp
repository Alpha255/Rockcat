#include "RHI/RHISwapchain.h"
#include "RHI/RHIDevice.h"
#include "Application/Window.h"
#include "Application/BaseApplication.h"

RHIViewWindow::RHIViewWindow(const Window& InWindow, bool VSync, bool HDR)
	: m_Window(InWindow)
{
	RHISwapchainDesc Desc;
	Desc.SetWidth(InWindow.GetWidth())
		.SetHeight(InWindow.GetHeight())
		.SetFullscreen(InWindow.IsFullscreen())
		.SetVSync(VSync)
		.SetHDR(HDR)
		.SetWindowHandle(InWindow.GetHandle());

	extern BaseApplication* GApplication;
	m_Swapchain = GApplication->GetRenderDevice().CreateSwapchain(Desc);
}

Math::UInt2 RHIViewWindow::GetViewSize() const
{
	return Math::UInt2(m_Window.GetWidth(), m_Window.GetHeight());
}
