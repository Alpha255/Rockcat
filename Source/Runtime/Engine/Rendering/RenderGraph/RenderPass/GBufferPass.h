#pragma once

#include "Colorful/IRenderer/RenderGraph/IFrameGraph.h"

NAMESPACE_START(RHI)

class GBufferPass : public OpaquePass
{
public:
	using OpaquePass::OpaquePass;
};

NAMESPACE_END(RHI)