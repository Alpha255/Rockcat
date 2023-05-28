#pragma once

#include "Colorful/Gfx/IRenderer.h"

NAMESPACE_START(Gfx)

class SoftwareRenderer final : public GfxRenderer
{
public:
	void initialize(uint64_t windowHandle, uint32_t width, uint32_t height, bool8_t fullscreen, bool8_t VSync) override;

	void toggleFullScreen(bool8_t fullscreen) override;
	void toggleVSync(bool8_t vSync) override;

	GfxDevice* device() override final
	{
		return nullptr;
	}

	void finalize() override;
protected:
private:
};

NAMESPACE_END(Gfx)