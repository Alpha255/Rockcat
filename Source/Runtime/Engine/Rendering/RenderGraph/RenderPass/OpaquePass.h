#pragma once

#include "Engine/Rendering/RenderGraph/RenderPass/MeshPass.h"

class OpaquePass : public MeshPass
{
public:
	OpaquePass(DAGNodeID ID, class ResourceManager& ResourceMgr)
		: MeshPass(ID, "OpaquePass", ResourceMgr, EMeshPass::Opaque)
	{
	}

	void Compile() override final {}
};
