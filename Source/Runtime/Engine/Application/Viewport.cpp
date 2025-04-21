#include "Engine/Application/Viewport.h"
#include "Engine/Services/RenderService.h"
#include "Engine/RHI/RHIBackend.h"
#include "Engine/RHI/RHISwapchain.h"
#include "Runtime/Core/Window.h"

RenderViewport::RenderViewport(Window& RenderWindow, bool Fullscreen, bool VSync)
	: m_Window(RenderWindow)
{
}

uint32_t RenderViewport::GetWidth() const
{
	return m_Window.GetWidth();
}

uint32_t RenderViewport::GetHeight() const
{
	return m_Window.GetHeight();
}

bool RenderViewport::IsFullscreen() const
{
	return false;
}

bool RenderViewport::IsVSync() const
{
	return false;
}

void RenderViewport::OnWindowResized(uint32_t Width, uint32_t Height)
{
}

void RenderViewport::SetFullscreen(bool Fullscreen)
{
	if (Fullscreen != IsFullscreen())
	{
		m_Window.SetMode(Fullscreen ? EWindowMode::ExclusiveFullscreen : EWindowMode::Windowed);
	}
}

void RenderViewport::SetVSync(bool VSync)
{
	if (VSync != IsVSync())
	{

	}
}
