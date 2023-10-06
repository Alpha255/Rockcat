#pragma once

#include "Runtime/Engine/Rendering/RenderGraph/RenderPass.h"

class DeferredLightingPass : public RenderPass
{
public:
	DECLARE_RENDERPASS_CONSTRUCTOR(DeferredLightingPass, RenderPass)

	void Execute(class RHIDevice&, const class Scene&) override final {}
};