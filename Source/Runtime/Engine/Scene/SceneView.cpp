#include "Engine/Scene/SceneView.h"
#include "Engine/Application/GraphicsSettings.h"

SceneView::SceneView()
	: m_InverseDepth(false)
{
	//SetViewport(RHIViewport(GfxSettings.Resolution.Width, GfxSettings.Resolution.Height));
	//SetScissorRect(RHIScissorRect(GfxSettings.Resolution.Width, GfxSettings.Resolution.Height));
}
