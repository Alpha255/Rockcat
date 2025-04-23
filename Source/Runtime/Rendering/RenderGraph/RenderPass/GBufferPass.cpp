#include "Rendering/RenderGraph/RenderPass/GBufferPass.h"
#include "Rendering/RenderGraph/RenderGraph.h"

GBufferPass::GBufferPass(DAGNodeID ID, RenderGraph& Graph)
	: GeometryPass(ID, "GBufferPass", Graph)
{
}
