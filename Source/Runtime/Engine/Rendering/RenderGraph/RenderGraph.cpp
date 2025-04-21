#include "Engine/Rendering/RenderGraph/RenderGraph.h"
#include "Engine/Rendering/RenderGraph/ResourceManager.h"
#include "Engine/Rendering/RenderGraph/ForwardRenderingPath.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Application/Viewport.h"
#include "Engine/RHI/RHIBackend.h"
#include "Engine/Services/RenderService.h"

std::shared_ptr<RenderGraph> RenderGraph::Create(const RenderSettings& GraphicsSettings, const RenderViewport& InRenderViewport)
{
	std::shared_ptr<RenderGraph> Graph;

	switch (GraphicsSettings.RenderingPath)
	{
	case ERenderingPath::ForwardRendering:
		Graph.reset(new ForwardRenderingPath(GraphicsSettings, InRenderViewport));
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

RenderGraph::RenderGraph(const RenderSettings& GraphicsSettings, const RenderViewport& InRenderViewport)
	: m_RenderDevice(RenderService::Get().GetBackend().GetDevice())
	, m_RenderSettings(GraphicsSettings)
	, m_RenderViewport(InRenderViewport)
	, m_ResourceMgr(new ResourceManager())
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

Math::UInt2 RenderGraph::GetDisplaySize() const
{
	return Math::UInt2(m_RenderViewport.GetWidth(), m_RenderViewport.GetHeight());
}

void RenderGraph::Execute(const Scene& InScene)
{
	if (!InScene.IsReady())
	{
		return;
	}

	if (!m_RenderScene || (&m_RenderScene->GetScene() != &InScene))
	{
		m_RenderScene = std::make_shared<RenderScene>(InScene);
	}
	m_RenderScene->BuildMeshDrawCommands(GetRenderDevice(), GetRenderSettings());

	Compile();

	m_ResourceMgr->ResolveResources(GetRenderDevice());

	for (auto& Pass : m_RenderPasses)
	{
		Pass->Execute(*m_RenderScene);
	}
}
