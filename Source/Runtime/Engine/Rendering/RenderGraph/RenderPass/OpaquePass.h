#pragma once

#include "Engine/Rendering/RenderGraph/RenderPass/GeometryPass.h"

class OpaquePass : public GeometryPass
{
public:
	OpaquePass(DAGNodeID ID, class ResourceManager& ResourceMgr)
		: GeometryPass(ID, "OpaquePass", ResourceMgr, EGeometryPass::Opaque)
	{
	}

	void Compile() override final {}
};
