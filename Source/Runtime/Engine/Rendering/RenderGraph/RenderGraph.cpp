#include "Engine/Rendering/RenderGraph/RenderGraph.h"
#include "Engine/Rendering/RenderGraph/ResourceManager.h"
#include "Engine/Rendering/RenderGraph/ForwardRenderingPath.h"
#include "Engine/Services/RenderService.h"
#include "Engine/Scene/Scene.h"

std::shared_ptr<RenderGraph> RenderGraph::Create(const GraphicsSettings& GfxSettings)
{
	auto& Backend = RenderService::Get().GetBackend(GfxSettings.Interface);
	std::shared_ptr<RenderGraph> Graph;

	switch (GfxSettings.RenderingPath)
	{
	case ERenderingPath::ForwardRendering:
		Graph.reset(new ForwardRenderingPath(Backend));
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

RenderGraph::RenderGraph(RHIInterface& Backend)
	: m_Backend(Backend)
	, m_ResourceMgr(new ResourceManager())
	, m_RenderScene(new RenderScene())
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
	if (InScene.IsDirty())
	{
		m_RenderScene->RebuildMeshDrawCommands(InScene, m_Backend.GetDevice(), m_Backend.GetGraphicsSettings().AsyncMeshDrawCommandsBuilding);
	}

	Compile();

	m_ResourceMgr->ResolveResources(m_Backend.GetDevice());

	for (auto& Pass : m_RenderPasses)
	{
		Pass->Execute(*m_RenderScene);
	}
}
