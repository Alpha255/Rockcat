#pragma once

#include "Colorful/IRenderer/RenderGraph/RenderPass/ForwardShadingPass.h"
#include "Colorful/IRenderer/RenderGraph/RenderPass/GBufferPass.h"
#include "Colorful/IRenderer/RenderGraph/RenderPass/DeferredLightingPass.h"
#include "Colorful/IRenderer/RenderGraph/RenderPass/SkyPass.h"
#include "Colorful/IRenderer/RenderGraph/RenderPass/PostProcessingPass.h"
#include "Colorful/IRenderer/RenderGraph/RenderPass/BlitPass.h"
#include "Colorful/IRenderer/RenderGraph/RenderPass/ImGuiPass.h"

NAMESPACE_START(RHI)

class SimpleFrameGraph : public IFrameGraph
{
public:
	using IFrameGraph::IFrameGraph;

	void Render(const RenderSettings* Settings, IFrameBuffer* SwapchainFrameBuffer) override final;
};

NAMESPACE_END(RHI)
