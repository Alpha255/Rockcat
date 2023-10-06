#include "Runtime/Engine/Rendering/RenderGraph/ForwardShadingPath.h"
#include "Runtime/Engine/Rendering/RenderGraph/RenderPass/PreDepthPass.h"
#include "Runtime/Engine/Rendering/RenderGraph/RenderPass/ShadowPass.h"
#include "Runtime/Engine/Rendering/RenderGraph/RenderPass/ForwardShadingPass.h"

void ForwardShadingPath::Setup()
{
	auto& PreDepth = CreateRenderPass<PreDepthPass>();
	auto& Shadow = CreateRenderPass<ShadowPass>();
	auto& ForwardShading = CreateRenderPass<ForwardShadingPass>();
}
