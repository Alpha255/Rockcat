#include "Engine/Scene/SceneView.h"
#include "Engine/Application/GraphicsSettings.h"

SceneView::SceneView(const GraphicsSettings& GfxSettings)
	: m_InverseDepth(GfxSettings.InverseDepth)
{
	SetViewport(RHIViewport(GfxSettings.Resolution.Width, GfxSettings.Resolution.Height));
	SetScissorRect(RHIScissorRect(GfxSettings.Resolution.Width, GfxSettings.Resolution.Height));
}
