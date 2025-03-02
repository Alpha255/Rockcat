#include "Engine/Rendering/RenderGraph/RenderGraph.h"
#include "Engine/Rendering/RenderGraph/ResourceManager.h"
#include "Engine/Rendering/RenderGraph/ForwardRenderingPath.h"
#include "Engine/Services/RenderService.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Application/Viewport.h"

std::shared_ptr<RenderGraph> RenderGraph::Create(RHIBackend& Backend, const RenderSettings& GraphicsSettings, const Viewport& RenderViewport)
{
	std::shared_ptr<RenderGraph> Graph;

	switch (GraphicsSettings.RenderingPath)
	{
	case ERenderingPath::ForwardRendering:
		Graph.reset(new ForwardRenderingPath(Backend, GraphicsSettings, RenderViewport));
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

RenderGraph::RenderGraph(RHIBackend& Backend, const RenderSettings& GraphicsSettings, const Viewport& RenderViewport)
	: m_Backend(Backend)
	, m_RenderSettings(GraphicsSettings)
	, m_RenderViewport(RenderViewport)
	, m_ResourceMgr(new ResourceManager())
{
	MessageRouter::Get().RegisterMessageHandler(this);
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

void RenderGraph::OnWindowResized(uint32_t Width, uint32_t Height)
{
	(void)Width;
	(void)Height;
}

void RenderGraph::Execute(const Scene& InScene)
{
#if 1
	if (!m_RenderScene)
	{
		m_RenderScene = std::make_shared<RenderScene>(InScene);
		m_RenderScene->BuildMeshDrawCommands(GetBackend().GetDevice(), GetRenderSettings());
		return;
	}

	if (&m_RenderScene->GetScene() != &InScene)
	{
		m_RenderScene = std::make_shared<RenderScene>(InScene);
	}

	m_RenderScene->BuildMeshDrawCommands(GetBackend().GetDevice(), GetRenderSettings());

	/// Make render scene one frame lag than main thread ???
#else
	if (!m_RenderScene || (&m_RenderScene->GetScene() != &InScene))
	{
		m_RenderScene = std::make_shared<RenderScene>(InScene);
	}
	m_RenderScene->BuildMeshDrawCommands(GetBackend().GetDevice(), GetRenderSettings());
#endif

	Compile();

	m_ResourceMgr->ResolveResources(m_Backend.GetDevice());

	for (auto& Pass : m_RenderPasses)
	{
		Pass->Execute(*m_RenderScene);
	}
}
