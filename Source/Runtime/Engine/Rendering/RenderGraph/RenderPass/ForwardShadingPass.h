#pragma once

#include "Runtime/Engine/Rendering/RenderGraph/RenderPass/MeshPass.h"

class ForwardShadingPass : public MeshPass
{
public:
	DECLARE_RENDERPASS_CONSTRUCTOR(ForwardShadingPass, MeshPass)
};
