#include "Engine/Application/Viewport.h"
#include "Engine/RHI/RHIBackend.h"
#include "Engine/RHI/RHISwapchain.h"
#include "Runtime/Core/Window.h"

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

	MessageRouter::Get().RegisterMessageHandler(this);
}

bool Viewport::IsFullscreen() const
{
	return m_Swapchain->IsFullscreen();
}

bool Viewport::IsVSync() const
{
	return m_Swapchain->IsVSync();
}

void Viewport::OnWindowResized(uint32_t Width, uint32_t Height)
{
	m_Swapchain->Resize(Width, Height);
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
