#include "Applications/RenderTest/RenderTest.h"
#include "Scene/Scene.h"
#include "Scene/SceneView.h"
#include "Rendering/RenderGraph/RenderGraph.h"
#include "Window.h"

void RenderTest::Initialize()
{
	m_Scene = Scene::Load("RenderTest.scene");
	m_Scene->AddView<PlanarSceneView>();
	//m_View = std::make_shared<PlanarView>();
	//m_View->SetCamera(&m_Scene->GetMainCamera());
	//m_View->SetViewport(RHIViewport(m_Window->GetWidth(), m_Window->GetHeight()));
	//m_View->SetScissorRect(RHIScissorRect(m_Window->GetWidth(), m_Window->GetHeight()));

	//m_RenderGraph = RenderGraph::Create(GetRenderSettings(), *m_View);
}

void RenderTest::Render(RHITexture* RenderSurface)
{
	//m_RenderGraph->Execute(*m_Scene);
}

RUN_APPLICATION(RenderTest, "RenderTest.json")
