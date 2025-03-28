#pragma once

#include "Core/Definitions.h"
#include "Core/Window.h"
#include "Core/MessageRouter.h"
#include "Engine/RHI/RHIDeclarations.h"

class Viewport : public MessageHandler
{
public:
	Viewport(class RHIDevice& Device, const struct WindowCreateInfo& WindowInfo, bool Fullscreen, bool VSync);

	uint32_t GetWidth() const { return m_Window->GetWidth(); }
	uint32_t GetHeight() const { return m_Window->GetHeight(); }
	bool IsFullscreen() const;
	bool IsVSync() const;

	void SetFullscreen(bool Fullscreen);
	void SetVSync(bool VSync);

	const Window& GetWindow() const { return *m_Window; }
	const RHISwapchain& GetSwapchain() const { return *m_Swapchain; }

	void OnWindowResized(uint32_t Width, uint32_t Height) override final;
private:
	std::unique_ptr<Window> m_Window;
	RHISwapchainPtr m_Swapchain;
};
