#include "Engine/Rendering/RenderGraph/ForwardRenderingPath.h"
#include "Engine/Rendering/RenderGraph/RenderPass/PreDepthPass.h"
#include "Engine/Rendering/RenderGraph/RenderPass/ShadowPass.h"
#include "Engine/Rendering/RenderGraph/RenderPass/OpaquePass.h"
#include "Engine/Rendering/RenderGraph/RenderPass/TranslucentPass.h"
#include "Engine/Rendering/RenderGraph/RenderPass/BlitPass.h"

void ForwardRenderingPath::SetupRenderPasses()
{
	AddPass<PreDepthPass>();
}
