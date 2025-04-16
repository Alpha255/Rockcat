#pragma once

#include "Core/MessageRouter.h"
#include "Engine/RHI/RHIDeclarations.h"

class Viewport : public MessageHandler
{
public:
	Viewport(const class Window& ViewWindow, bool Fullscreen, bool VSync);

	uint32_t GetWidth() const;
	uint32_t GetHeight() const;
	bool IsFullscreen() const;
	bool IsVSync() const;

	void SetFullscreen(bool Fullscreen);
	void SetVSync(bool VSync);

	const RHISwapchain& GetSwapchain() const { return *m_Swapchain; }

	void OnWindowResized(uint32_t Width, uint32_t Height) override final;
private:
	const class Window& m_Window;
	RHISwapchainPtr m_Swapchain;
};
