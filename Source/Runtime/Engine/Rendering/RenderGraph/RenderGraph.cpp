#include "Engine/Rendering/RenderGraph/RenderGraph.h"
#include "Engine/Rendering/RenderGraph/ResourceManager.h"
#include "Engine/RHI/RHIInterface.h"
#include "Engine/Scene/Scene.h"

RenderGraph::RenderGraph(RHIInterface& RHI, const Scene& InScene)
	: m_RenderScene(std::make_shared<RenderScene>(InScene))
	, m_ResourceMgr(std::move(std::make_shared<ResourceManager>(RHI, m_Graph)))
{
}

void RenderGraph::Compile()
{
	if (m_NeedRecompile)
	{
		for (auto& Pass : m_RenderPasses)
		{
			Pass->Compile();
		}

		m_RenderScene->GenerateDrawCommands();

		m_NeedRecompile = false;
	}
}

void RenderGraph::Execute()
{
	Compile();

	m_ResourceMgr->CreateAllResources();

	for (auto& Pass : m_RenderPasses)
	{
		Pass->Execute(m_ResourceMgr->GetRHI().GetDevice(), *m_RenderScene);
	}
}
