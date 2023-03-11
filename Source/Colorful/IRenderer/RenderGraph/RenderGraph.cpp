#include "Colorful/IRenderer/RenderGraph/RenderGraph.h"
#include "Colorful/IRenderer/RenderSettings.h"
#include "Colorful/IRenderer/IRenderer.h"
#include "Runtime/Scene.h"

NAMESPACE_START(RHI)

RenderGraph::RenderGraph(bool8_t EnableAsyncTasks)
	: m_EnableAsyncTasks(EnableAsyncTasks)
{
	m_RenderPasses.reserve(std::numeric_limits<uint8_t>().max());
}

void RenderGraph::AddPass(const std::shared_ptr<IRenderPass>& RenderPass)
{
	assert(RenderPass);

	m_Graph.AddNode(RenderPass);

	if (RenderPass->ID().Index() > m_RenderPasses.size())
	{
		m_RenderPasses.emplace_back();
	}
	m_RenderPasses[RenderPass->ID().Index()] = RenderPass;

	m_NeedRecompile = true;
}

void RenderGraph::RemovePass(RenderPassID PassID)
{
	assert(PassID.IsValid() && PassID.Index() < m_RenderPasses.size());

	m_Graph.RemoveNode(PassID);
	m_RenderPasses[PassID.Index()].reset();

	m_NeedRecompile = true;
}

std::shared_ptr<IRenderPass> RenderGraph::GetPass()
{
	return std::shared_ptr<IRenderPass>();
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
