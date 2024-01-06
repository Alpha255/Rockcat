#pragma once

#include "Runtime/Engine/Rendering/RenderGraph/RenderPass/MeshPass.h"

class ShadowPass : public MeshPass
{
public:
	ShadowPass(DAGNodeID ID, class ResourceManager& ResourceMgr);

	void Compile() override final {}
protected:
private:
};
