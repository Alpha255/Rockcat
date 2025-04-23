#pragma once

#include "Rendering/RenderGraph/RenderPass.h"

class DeferredLightingPass : public RenderPass
{
public:
	void Execute(const RenderScene&) override final {}
};