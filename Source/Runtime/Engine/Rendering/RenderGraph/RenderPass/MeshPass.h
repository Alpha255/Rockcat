#pragma once

#include "Runtime/Engine/Rendering/RenderGraph/RenderPass.h"

class MeshPass : public RenderPass
{
public:
	using RenderPass::RenderPass;

	void Execute(class RHIDevice&, const class Scene&);
};
