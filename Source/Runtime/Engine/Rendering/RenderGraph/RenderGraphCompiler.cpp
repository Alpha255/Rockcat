#include "Runtime/Engine/Rendering/RenderGraph/RenderGraphCompiler.h"
#include "Runtime/Engine/Rendering/RenderGraph/RenderGraph.h"

RenderGraphCompiler::RenderGraphCompiler(RenderGraph& Graph)
	: m_Graph(Graph)
{
}

void RenderGraphCompiler::Compile()
{
}

void RenderGraphCompiler::InsertResourceBarrierPass()
{
}

void RenderGraphCompiler::InsertResolvePass()
{
}
