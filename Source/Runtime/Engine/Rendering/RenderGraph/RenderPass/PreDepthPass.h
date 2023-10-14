#pragma once

#include "Runtime/Engine/Rendering/RenderGraph/RenderPass/MeshPass.h"

class PreDepthPass : public MeshPass
{
public:
	PreDepthPass(DAGNodeID ID, class ResourceManager& ResourceMgr)
		: MeshPass(ID, "PreDepthPass", ResourceMgr, MeshPass::EMeshDrawFilter::All)
	{
	}
};
