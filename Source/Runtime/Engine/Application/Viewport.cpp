#include "Engine/Application/Viewport.h"
#include "Runtime/Core/Window.h"
#include "Engine/RHI/RHIBackend.h"
#include "Engine/RHI/RHISwapchain.h"

Viewport::Viewport(RHIBackend& Backend, const WindowCreateInfo& WindowInfo, bool Fullscreen, bool VSync)
	: m_Window(new Window(WindowInfo))
{
	m_Window->SetMode(Fullscreen ? EWindowMode::ExclusiveFullscreen : EWindowMode::Windowed);

	RHISwapchainCreateInfo CreateInfo;
	CreateInfo.SetWindowHandle(m_Window->GetHandle())
		.SetWidth(m_Window->GetWidth())
		.SetHeight(m_Window->GetHeight())
		.SetFullscreen(Fullscreen)
		.SetVSync(VSync);
	m_Swapchain = Backend.GetDevice().CreateSwapchain(CreateInfo);
}

bool Viewport::IsFullscreen() const
{
	return m_Swapchain->IsFullscreen();
}

bool Viewport::IsVSync() const
{
	return m_Swapchain->IsVSync();
}

void Viewport::SetFullscreen(bool Fullscreen)
{
	if (Fullscreen != IsFullscreen())
	{

	}
}

void Viewport::SetVSync(bool VSync)
{
	if (VSync != IsVSync())
	{

	}
}
