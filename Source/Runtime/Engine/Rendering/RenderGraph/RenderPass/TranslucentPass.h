#pragma once

#include "Engine/Rendering/RenderGraph/RenderPass/GeometryPass.h"

class TranslucentPass : public GeometryPass
{
public:
	TranslucentPass(DAGNodeID ID, class ResourceManager& ResourceMgr)
		: GeometryPass(ID, "TranslucentPass", ResourceMgr, EGeometryPass::Translucent)
	{
	}

	void Compile() override final {}
};