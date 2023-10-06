#pragma once

#include "Runtime/Engine/Rendering/RenderGraph/RenderPass/MeshPass.h"

class TransparencyPass : public MeshPass
{
public:
	TransparencyPass(DAGNodeID ID, class ResourceManager& ResourceMgr)
		: MeshPass(ID, "TransparencyPass", ResourceMgr)
	{
	}
};