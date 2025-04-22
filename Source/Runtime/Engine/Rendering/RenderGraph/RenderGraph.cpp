#include "Engine/Rendering/RenderGraph/RenderGraph.h"
#include "Engine/Rendering/RenderGraph/ResourceManager.h"
#include "Engine/Rendering/RenderGraph/ForwardRenderingPath.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Application/Viewport.h"
#include "Engine/RHI/RHIBackend.h"
#include "Engine/Services/RenderService.h"

std::shared_ptr<RenderGraph> RenderGraph::Create(const RenderSettings& InRenderSettings)
{
	std::shared_ptr<RenderGraph> Graph;

	switch (InRenderSettings.RenderingPath)
	{
	case ERenderingPath::ForwardRendering:
		Graph.reset(new ForwardRenderingPath(InRenderSettings));
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

RenderGraph::RenderGraph(const RenderSettings& InRenderSettings)
	: m_Device(RenderService::Get().GetBackend().GetDevice())
	, m_RenderSettings(InRenderSettings)
	, m_ResourceMgr(new ResourceManager(m_Device))
	, m_DisplaySize(0u, 0u)
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

void RenderGraph::SetDisplaySize(uint32_t Width, uint32_t Height)
{
	if (Width != m_DisplaySize.x || Height != m_DisplaySize.y)
	{
		m_DisplaySize = Math::UInt2(Width, Height);
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
	m_RenderScene->BuildMeshDrawCommands(GetRenderSettings());

	Compile();

	m_ResourceMgr->ResolveResources();

	for (auto& Pass : m_RenderPasses)
	{
		Pass->Execute(*m_RenderScene);
	}
}
