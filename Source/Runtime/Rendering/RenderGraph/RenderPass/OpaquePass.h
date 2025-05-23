#pragma once

#include "Rendering/RenderGraph/RenderPass/GeometryPass.h"

class OpaquePass : public GeometryPass
{
public:
	OpaquePass(DAGNodeID ID, class RenderGraph& Graph);

	void Compile() override final {}
};
