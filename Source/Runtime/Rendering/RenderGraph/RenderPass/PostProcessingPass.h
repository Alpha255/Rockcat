#pragma once

#include "Rendering/RenderGraph/RenderPass.h"

class PostProcessingPass : public RenderPass
{
public:
	void Compile() override final {}

	void Execute(const RenderScene&) override final {}
};
