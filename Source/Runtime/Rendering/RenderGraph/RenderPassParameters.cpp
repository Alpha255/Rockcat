#include "Rendering/RenderGraph/RenderPassParameters.h"

void RDGSceneTextures::InitializeWithSceneView(RDGRenderGraph& GraphBuilder, SceneView& View)
{
}

uint32_t RDGSceneTextures::GetSceneTextureBindingSlots(RDGRenderTargetBindingSlots& Slots, bool WithDepthStencil) const
{
	return 0;
}

uint32_t RDGSceneTextures::GetGBufferBindingSlots(RDGRenderTargetBindingSlots& Slots) const
{
	return 0;
}

RDGSceneViewInfo::RDGSceneViewInfo(const SceneView& View)
	: ViewMode(View.GetViewMode())
	, OriginalViewSize(View.GetViewSize())
	, DesiredViewSize(View.GetViewSize())
	, FinalOutput(View.GetRenderSurface())
{
}
