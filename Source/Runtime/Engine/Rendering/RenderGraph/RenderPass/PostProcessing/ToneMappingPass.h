#pragma once

#include "Colorful/IRenderer/RenderGraph/IFrameGraph.h"

NAMESPACE_START(RHI)

class ToneMappingPass : public IFrameGraphPass
{
public:
	using IFrameGraphPass::IFrameGraphPass;

	void ApplyRenderSettings(const RenderSettings*) override final {}
	void Render(class ICommandBuffer*) override final {}
	void SetupMaterial(const MeshInstance*) override final {}
};

NAMESPACE_END(RHI)