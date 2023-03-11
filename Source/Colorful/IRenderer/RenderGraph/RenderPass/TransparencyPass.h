#pragma once

#include "Colorful/IRenderer/RenderGraph/IFrameGraph.h"

NAMESPACE_START(RHI)

class TransparencyPass : public IFrameGraphPass
{
public:
	using IFrameGraphPass::IFrameGraphPass;
};

NAMESPACE_END(RHI)