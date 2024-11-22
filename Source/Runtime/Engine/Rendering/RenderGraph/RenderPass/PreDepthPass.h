#pragma once

#include "Engine/Rendering/RenderGraph/RenderPass/GeometryPass.h"

class PreDepthPass : public GeometryPass
{
public:
	PreDepthPass(DAGNodeID ID, class RenderGraph& Graph);

	void Compile() override final;
};
