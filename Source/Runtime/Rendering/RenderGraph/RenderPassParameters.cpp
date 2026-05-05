#include "Rendering/RenderGraph/RenderPassParameters.h"
#include "Rendering/RenderGraph/RenderGraph.h"

void RDGSceneTextures::InitializeWithSceneView(RDGRenderGraph& Graph, const SceneView& View)
{
	if (Graph.GetRenderSettings().RenderingPath == ERenderingPath::DeferredShading)
	{
		//GBuffers.BaseColor = Graph.CreateTexture();
	}
	else
	{

	}
}

uint32_t RDGSceneTextures::GetSceneTextureBindingSlots(RDGRenderTargetBindingSlots& Slots, bool WithDepthStencil) const
{
	uint32_t NumRenderTargets = 0u;

	Slots.Colors[0].SetTexture(Color.Target)
		.SetResolveTexture(Color.ResolveTarget);
	++NumRenderTargets;

	return NumRenderTargets;
}

uint32_t RDGSceneTextures::GetGBufferBindingSlots(RDGRenderTargetBindingSlots& Slots) const
{
	return 0;
}

RDGSceneViewInfo::RDGSceneViewInfo(RDGRenderGraph& Graph, const RHIViewWindow& ViewWindow)
{
	//SceneTextures.InitializeWithSceneView(Graph, View);
}
