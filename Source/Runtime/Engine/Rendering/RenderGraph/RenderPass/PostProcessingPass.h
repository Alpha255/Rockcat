#pragma once

#include "Engine/Rendering/RenderGraph/RenderPass.h"

class PostProcessingPass : public RenderPass
{
public:
	DECLARE_RENDERPASS_CONSTRUCTOR(PostProcessingPass, RenderPass)

	void Compile() override final {}

	void Execute(class RHIDevice&, const RenderScene&) override final {}
};
