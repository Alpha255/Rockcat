#pragma once

#include "Runtime/Engine/Rendering/RenderGraph/RenderPass/MeshPass.h"

class GeometryBufferPass : public MeshPass
{
public:
	DECLARE_RENDERPASS_CONSTRUCTOR(GeometryBufferPass, MeshPass)
};