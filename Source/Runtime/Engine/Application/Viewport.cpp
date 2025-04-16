#include "Engine/Application/Viewport.h"
#include "Engine/Services/RenderService.h"
#include "Engine/RHI/RHIBackend.h"
#include "Engine/RHI/RHISwapchain.h"
#include "Runtime/Core/Window.h"

Viewport::Viewport(const Window& ViewWindow, bool Fullscreen, bool VSync)
	: m_Window(ViewWindow)
{
	RHISwapchainCreateInfo CreateInfo;
	CreateInfo.SetWindowHandle(m_Window.GetHandle())
		.SetWidth(m_Window.GetWidth())
		.SetHeight(m_Window.GetHeight())
		.SetFullscreen(Fullscreen)
		.SetVSync(VSync);
	m_Swapchain = RenderService::Get().GetBackend().GetDevice().CreateSwapchain(CreateInfo);

	MessageRouter::Get().RegisterMessageHandler(this);
}

uint32_t Viewport::GetWidth() const
{
	return m_Window.GetWidth();
}

uint32_t Viewport::GetHeight() const
{
	return m_Window.GetHeight();
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
