#pragma once

#include "Engine/Rendering/RenderGraph/RenderPass/GeometryPass.h"

class TranslucentPass : public GeometryPass
{
public:
	TranslucentPass(DAGNodeID ID, class RenderGraph& Graph);

	void Compile() override final {}
};