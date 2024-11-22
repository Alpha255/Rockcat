#pragma once

#include "Engine/Rendering/RenderGraph/RenderPass/GeometryPass.h"
#include "Engine/Asset/GlobalShaders/DefaultShading.h"

class GBufferPass : public GeometryPass
{
protected:
public:
	GBufferPass(DAGNodeID ID, class RenderGraph& Graph);

	void Compile() override final {}
};