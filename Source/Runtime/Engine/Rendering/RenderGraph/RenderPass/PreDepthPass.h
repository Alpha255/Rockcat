#pragma once

#include "Engine/Rendering/RenderGraph/RenderPass/GeometryPass.h"

class PreDepthPass : public GeometryPass
{
public:
	PreDepthPass(DAGNodeID ID, class ResourceManager& ResourceMgr)
		: GeometryPass(ID, "PreDepthPass", ResourceMgr, EGeometryPassFilter::PreDepth)
	{
	}

	void Compile() override final;
};
