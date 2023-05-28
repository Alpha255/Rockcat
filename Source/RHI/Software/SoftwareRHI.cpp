#include "SoftwareRenderer.h"

NAMESPACE_START(Gfx)

void SoftwareRenderer::initialize(uint64_t windowHandle, uint32_t width, uint32_t height, bool8_t fullscreen, bool8_t VSync)
{
	(void)windowHandle;
	(void)width;
	(void)height;
	(void)fullscreen;
	(void)VSync;
}

void SoftwareRenderer::toggleFullScreen(bool8_t fullscreen)
{
	(void)fullscreen;
}

void SoftwareRenderer::toggleVSync(bool8_t VSync)
{
	(void)VSync;
}

void SoftwareRenderer::finalize()
{
}

extern "C"
{
	EXPORT_API void createRenderer(GfxRendererPtr& ptr)
	{
		ptr = std::make_unique<SoftwareRenderer>();
	}
}

NAMESPACE_END(Gfx)
