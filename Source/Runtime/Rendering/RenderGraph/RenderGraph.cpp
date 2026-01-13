#include "Rendering/RenderGraph/RenderGraph.h"
#include "Rendering/RenderGraph/ResourceManager.h"
#include "Rendering/RenderGraph/ForwardRenderingPath.h"
#include "Scene/Scene.h"
#include "Rendering/SceneView.h"

std::shared_ptr<RenderGraph> RenderGraph::Create(const RenderSettings& InRenderSettings, RHIDevice& Device, const ISceneView& InView)
{
	std::shared_ptr<RenderGraph> Graph;

	switch (InRenderSettings.RenderingPath)
	{
	case ERenderingPath::ForwardRendering:
		Graph.reset(new ForwardRenderingPath(InRenderSettings, Device, InView));
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

RenderGraph::RenderGraph(const RenderSettings& InRenderSettings, RHIDevice& Device, const ISceneView& InView)
	: m_Device(Device)
	, m_SceneView(InView)
	, m_RenderSettings(InRenderSettings)
	, m_ResourceMgr(new ResourceManager(m_Device))
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
	if (!InScene.IsReady())
	{
		return;
	}

	if (!m_RenderScene || (&m_RenderScene->GetScene() != &InScene))
	{
		m_RenderScene = std::make_shared<RenderScene>(InScene, m_RenderSettings.EnableAsyncMeshDrawCommandsBuilding);
	}
	m_RenderScene->BuildMeshDrawCommands(m_SceneView);

	Compile();

	m_ResourceMgr->ResolveResources();

	for (auto& Pass : m_RenderPasses)
	{
		Pass->Execute(*m_RenderScene);
	}
}
