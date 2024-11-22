#pragma once

#include "Engine/Rendering/RenderGraph/RenderPass/GeometryPass.h"

class ShadowPass : public GeometryPass
{
public:
	ShadowPass(DAGNodeID ID, class RenderGraph& Graph);

	void Compile() override final {}
protected:
private:
};
