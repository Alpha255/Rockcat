#include "Engine/View/View.h"
#include "Engine/Application/GraphicsSettings.h"

IView::IView(const GraphicsSettings& GfxSettings)
{
	SetViewport(RHIViewport(GfxSettings.Resolution.Width, GfxSettings.Resolution.Height));
	SetScissorRect(RHIScissorRect(GfxSettings.Resolution.Width, GfxSettings.Resolution.Height));
}
