#include "Engine/Rendering/RenderGraph/RenderGraph.h"
#include "Engine/Rendering/RenderGraph/ResourceManager.h"
#include "Engine/Rendering/RenderGraph/ForwardRenderingPath.h"
#include "Engine/Scene/Scene.h"

std::shared_ptr<RenderGraph> RenderGraph::Create(const GraphicsSettings& GfxSettings)
{
	std::shared_ptr<RenderGraph> Graph;
	switch (GfxSettings.RenderingPath)
	{
	case ERenderingPath::ForwardRendering:
		Graph = std::make_shared<ForwardRenderingPath>(GfxSettings);
		break;
	case ERenderingPath::DeferredShading:
		assert(false);
		break;
	case ERenderingPath::DeferredLighting:
		assert(false);
		break;
	}

	Graph->SetupRenderPasses();
	return Graph;
}

RenderGraph::RenderGraph(const GraphicsSettings& GfxSettings)
	: m_GfxSettings(GfxSettings)
	, m_ResourceMgr(std::move(std::make_shared<ResourceManager>(GfxSettings, m_Graph)))
{
}

void RenderGraph::Compile()
{
	if (m_Dirty)
	{
		for (auto& Pass : m_RenderPasses)
		{
			Pass->Compile();
		}

		SetDirty(false);
	}
}

void RenderGraph::Execute(const Scene& InScene)
{
	if (!m_RenderScene || InScene.IsDirty())
	{
		m_RenderScene = std::make_shared<RenderScene>(InScene);
	}

	Compile();

	m_ResourceMgr->ResolveResources();

	for (auto& Pass : m_RenderPasses)
	{
		//Pass->Execute(m_ResourceMgr->GetRHI().GetDevice(), *m_RenderScene);
	}
}
