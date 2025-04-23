#include "Rendering/RenderGraph/ForwardRenderingPath.h"
#include "Rendering/RenderGraph/RenderPass/PreDepthPass.h"
#include "Rendering/RenderGraph/RenderPass/ShadowPass.h"
#include "Rendering/RenderGraph/RenderPass/OpaquePass.h"
#include "Rendering/RenderGraph/RenderPass/TranslucentPass.h"
#include "Rendering/RenderGraph/RenderPass/BlitPass.h"

void ForwardRenderingPath::SetupRenderPasses()
{
	AddPass<PreDepthPass>();
}
