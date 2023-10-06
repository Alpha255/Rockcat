#pragma once

#include "Runtime/Engine/Rendering/RenderGraph/RenderPass/MeshPass.h"

class PreDepthPass : public MeshPass
{
public:
	DECLARE_RENDERPASS_CONSTRUCTOR(PreDepthPass, MeshPass)
};
