#pragma once

#include "Runtime/Engine/Rendering/RenderGraph/RenderPass/MeshPass.h"

class TranslucentPass : public MeshPass
{
public:
	TranslucentPass(DAGNodeID ID, class ResourceManager& ResourceMgr)
		: MeshPass(ID, "TranslucentPass", ResourceMgr, MeshPass::EMeshDrawFilter::Translucent)
	{
	}

	void Compile() override final {}
};