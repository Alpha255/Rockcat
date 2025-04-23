#pragma once

#include "Rendering/RenderGraph/RenderGraph.h"

class ForwardRenderingPath : public RenderGraph
{
public:
	using RenderGraph::RenderGraph;

protected:
	void SetupRenderPasses() override final;
};
