#include "Runtime/Engine/Rendering/RenderGraph/ForwardRenderingPath.h"
#include "Runtime/Engine/Rendering/RenderGraph/RenderPass/PreDepthPass.h"
#include "Runtime/Engine/Rendering/RenderGraph/RenderPass/ShadowPass.h"
#include "Runtime/Engine/Rendering/RenderGraph/RenderPass/OpaquePass.h"
#include "Runtime/Engine/Rendering/RenderGraph/RenderPass/TranslucentPass.h"
#include "Runtime/Engine/Rendering/RenderGraph/RenderPass/BlitPass.h"

void ForwardRenderingPath::Setup()
{
	auto& PreDepthNode = AddPass<PreDepthPass>();
	auto& ShadowNode = AddPass<ShadowPass>();
	auto& OpaqueNode = AddPass<OpaquePass>();
	auto& TranslucentNode = AddPass<TranslucentPass>();
	auto& FinalBlitNode = AddPass<BlitPass>();
}
