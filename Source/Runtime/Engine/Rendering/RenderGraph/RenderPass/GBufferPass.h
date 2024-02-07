#pragma once

#include "Engine/Rendering/RenderGraph/RenderPass/MeshPass.h"

class GeometryBufferPass : public MeshPass
{
public:
	GeometryBufferPass(DAGNodeID ID, class ResourceManager& ResourceMgr)
		: MeshPass(ID, "GBufferPass", ResourceMgr, EMeshPass::Opaque)
	{
	}

	void Compile() override final {}
};