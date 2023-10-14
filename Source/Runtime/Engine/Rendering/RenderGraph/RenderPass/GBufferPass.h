#pragma once

#include "Runtime/Engine/Rendering/RenderGraph/RenderPass/MeshPass.h"

class GeometryBufferPass : public MeshPass
{
public:
	GeometryBufferPass(DAGNodeID ID, class ResourceManager& ResourceMgr)
		: MeshPass(ID, "GBufferPass", ResourceMgr, MeshPass::EMeshDrawFilter::Opaque)
	{
	}
};