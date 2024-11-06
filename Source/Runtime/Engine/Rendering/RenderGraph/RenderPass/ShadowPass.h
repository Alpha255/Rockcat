#pragma once

#include "Engine/Rendering/RenderGraph/RenderPass/GeometryPass.h"

class ShadowPass : public GeometryPass
{
public:
	ShadowPass(DAGNodeID ID, class ResourceManager& ResourceMgr);

	void Compile() override final {}
protected:
private:
};
