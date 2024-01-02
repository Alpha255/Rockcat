#include "Runtime/Engine/Rendering/RenderGraph/RenderGraph.h"
#include "Runtime/Engine/Rendering/RenderGraph/ResourceManager.h"
#include "Runtime/Engine/Rendering/RenderGraph/RenderGraphCompiler.h"
#include "Runtime/Engine/RHI/RHIInterface.h"
#include "Runtime/Engine/RHI/RHIDevice.h"
#include "Runtime/Engine/Scene/Scene.h"

RenderGraph::RenderGraph(RHIInterface& RHI, Scene& RenderScene)
	: m_RenderDevice(RHI.GetDevice())
	, m_RenderScene(RenderScene)
	, m_ResourceMgr(std::move(std::make_shared<ResourceManager>(RHI.GetDevice(), m_Graph)))
{
}

void RenderGraph::Compile()
{
	if (m_NeedRecompile)
	{
		RenderGraphCompiler(*this).Compile();

		m_NeedRecompile = false;
	}
}

void RenderGraph::Execute()
{
	Compile();

	m_ResourceMgr->CreateAllResources();
}
