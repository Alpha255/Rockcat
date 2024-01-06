#pragma once

#include "Runtime/Engine/Rendering/RenderGraph/RenderPass/MeshPass.h"

class OpaquePass : public MeshPass
{
public:
	OpaquePass(DAGNodeID ID, class ResourceManager& ResourceMgr)
		: MeshPass(ID, "OpaquePass", ResourceMgr, MeshPass::EMeshDrawFilter::Opaque)
	{
	}

	void Compile() override final {}
};
