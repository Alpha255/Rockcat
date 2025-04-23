#include "Applications/RenderTest/RenderTest.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Scene/SceneView.h"
#include "Engine/Rendering/RenderGraph/RenderGraph.h"
#include "Engine/RHI/RHIBackend.h"
#include "Core/Window.h"

void RenderTest::InitializeImpl()
{
	m_Scene = Scene::Load<Scene>("RenderTest.scene");
	m_View = std::make_shared<PlanarView>();
	m_View->SetCamera(&m_Scene->GetMainCamera());
	m_View->SetViewport(RHIViewport(GetWindow().GetWidth(), GetWindow().GetHeight()));
	m_View->SetScissorRect(RHIScissorRect(GetWindow().GetWidth(), GetWindow().GetHeight()));

	m_RenderGraph = RenderGraph::Create(GetRenderSettings(), *m_View);
}

void RenderTest::RenderFrame()
{
	m_RenderGraph->Execute(*m_Scene);
}

RUN_APPLICATION(RenderTest, "RenderTest.json")
