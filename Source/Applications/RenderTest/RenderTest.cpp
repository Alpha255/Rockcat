#include "Applications/RenderTest/RenderTest.h"
#include "Scene/Scene.h"
#include "Scene/SceneView.h"
#include "Rendering/RenderGraph/RenderGraph.h"
#include "Window.h"

void RenderTest::Initialize()
{
	//m_Scene = Scene::Load<Scene>("RenderTest.scene");
	//m_View = std::make_shared<PlanarView>();
	//m_View->SetCamera(&m_Scene->GetMainCamera());
	//m_View->SetViewport(RHIViewport(GetWindow().GetWidth(), GetWindow().GetHeight()));
	//m_View->SetScissorRect(RHIScissorRect(GetWindow().GetWidth(), GetWindow().GetHeight()));

	//m_RenderGraph = RenderGraph::Create(GetRenderSettings(), *m_View);
}

void RenderTest::Render(RHITexture* RenderSurface)
{
	//m_RenderGraph->Execute(*m_Scene);
}

//RUN_APPLICATION(RenderTest, "RenderTest.json")
