#include "Engine/RHI/RHIViewport.h"

RHIViewport::RHIViewport(RHIDevice& Device, const void* WindowHandle, uint32_t Width, uint32_t Height, bool VSync, bool Fullscreen)
	: m_VSync(VSync)
	, m_Fullscreen(Fullscreen)
	, m_Width(Width)
	, m_Height(Height)
	, m_NumBackBuffer(1u)
	, m_BackBufferIndex(0u)
	, m_WindowHandle(WindowHandle)
{
}