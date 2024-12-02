#pragma once

#include "Engine/Rendering/RenderGraph/RenderPass.h"

class DeferredLightingPass : public RenderPass
{
public:
	DECLARE_RENDERPASS_CONSTRUCTOR(DeferredLightingPass, RenderPass)

	void Execute(const RenderScene&) override final {}
};