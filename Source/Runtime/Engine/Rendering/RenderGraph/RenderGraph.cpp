#if 0
#include "Colorful/IRenderer/RenderGraph/RenderGraph.h"
#include "Colorful/IRenderer/RenderSettings.h"
#include "Colorful/IRenderer/IRenderer.h"
#include "Runtime/Scene/Scene.h"

NAMESPACE_START(RHI)

RenderGraph::RenderGraph(bool8_t EnableAsyncTasks)
	: m_EnableAsyncTasks(EnableAsyncTasks)
{
	m_RenderPasses.reserve(std::numeric_limits<uint8_t>().max());
}

void RenderGraph::AddRenderPass(const IRenderPassSharedPtr& RenderPass)
{
	assert(RenderPass);

	m_Graph.AddNode(RenderPass);

	if (RenderPass->GetID().GetIndex() > m_RenderPasses.size())
	{
		m_RenderPasses.emplace_back();
	}
	m_RenderPasses[RenderPass->GetID().GetIndex()] = RenderPass;

	m_NeedRecompile = true;
}

void RenderGraph::RemoveRenderPass(RenderPassID PassID)
{
	assert(PassID.IsValid() && PassID.GetIndex() < m_RenderPasses.size());

	m_Graph.RemoveNode(PassID);
	m_RenderPasses[PassID.GetIndex()].reset();

	m_NeedRecompile = true;
}

IRenderPassSharedPtr RenderGraph::GetRenderPass()
{
	return IRenderPassSharedPtr();
}

void RenderGraph::Compile()
{
	if (m_NeedRecompile)
	{

	}
}

void RenderGraph::Execute()
{
}

NAMESPACE_END(RHI)

#endif
