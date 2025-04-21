#pragma once

#include "Core/MessageRouter.h"
#include "Engine/RHI/RHIDeclarations.h"

class RenderViewport : public MessageHandler
{
public:
	RenderViewport(class Window& RenderWindow, bool Fullscreen, bool VSync);

	uint32_t GetWidth() const;
	uint32_t GetHeight() const;
	bool IsFullscreen() const;
	bool IsVSync() const;

	void SetFullscreen(bool Fullscreen);
	void SetVSync(bool VSync);

	void OnWindowResized(uint32_t Width, uint32_t Height) override final;
private:
	class Window& m_Window;
};
