#include "Engine/Rendering/RenderGraph/RenderPass/GBufferPass.h"
#include "Engine/Rendering/RenderGraph/RenderGraph.h"

GBufferPass::GBufferPass(DAGNodeID ID, RenderGraph& Graph)
	: GeometryPass(ID, "GBufferPass", Graph)
{
}
