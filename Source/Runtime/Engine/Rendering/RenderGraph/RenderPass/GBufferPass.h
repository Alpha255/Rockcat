#pragma once

#include "Engine/Rendering/RenderGraph/RenderPass/GeometryPass.h"

class GBufferPass : public GeometryPass
{
public:
	GBufferPass(DAGNodeID ID, class ResourceManager& ResourceMgr)
		: GeometryPass(ID, "GBufferPass", ResourceMgr, EGeometryPassFilter::Opaque)
	{
	}

	void Compile() override final {}
};