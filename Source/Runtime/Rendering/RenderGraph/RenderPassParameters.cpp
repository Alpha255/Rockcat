#include "Rendering/RenderGraph/RenderPassParameters.h"
#include "Rendering/RenderGraph/RenderGraph.h"
#include "RHI/RHISwapchain.h"

void RDGSceneTextures::InitializeWithSceneView(RDGRenderGraph& Graph, const RDGSceneViewInfo& ViewInfo)
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

RDGSceneViewInfo::RDGSceneViewInfo(RDGRenderGraph& Graph, const RHIViewportClient& ViewportClient, const Scene& InScene)
	: SceneProxy(InScene)
{
	SetupSceneViews(ViewportClient);

	SceneTextures.InitializeWithSceneView(Graph, *this);
}

void RDGSceneViewInfo::SetupSceneViews(const RHIViewportClient& ViewportClient)
{
	FinalViewSize = OriginalViewSize = ViewportClient.GetViewSize();
	/// Scale the view rect to match the final output size if necessary.

	OriginalViewRect = ViewRect(0, 0, OriginalViewSize.x, OriginalViewSize.y);
	FinalViewRect = ViewRect(0, 0, FinalViewSize.x, FinalViewSize.y);

	uint32_t NumViews = ViewportClient.IsStereoRendering() ? 2u : 1u;
	Views.resize(NumViews);

	for (uint32_t i = 0u; i < NumViews; ++i)
	{
		if (ViewportClient.IsStereoRendering())
		{
			Views[i] = std::make_unique<StereoSceneView>();
		}
		else
		{
			Views[i] = std::make_unique<PlanarSceneView>();
		}
	}
}
