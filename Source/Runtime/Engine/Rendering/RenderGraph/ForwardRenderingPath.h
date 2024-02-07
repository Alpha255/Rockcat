#pragma once

#include "Engine/Rendering/RenderGraph/RenderGraph.h"

class ForwardRenderingPath : public RenderGraph
{
public:
	using RenderGraph::RenderGraph;

	void Setup() override final;
};
