#include "Applications/RenderTest/RenderTest.h"
#include "Application/ApplicationManager.h"
#include "Scene/Scene.h"
#include "Scene/SceneView.h"
#include "Rendering/RenderGraph/RenderGraph.h"

#include "Asset/GlobalShaders.h"
#include "Scene/Components/TransformComponent.h"

void RenderTest::Initialize()
{
	m_Scene = Scene::Load(Paths::ScenePath() / "RenderTest.json");

	//m_Scene->AddEntity(EntityID::NullIndex, "entity0");
	//m_Scene->AddEntity(EntityID::NullIndex, "entity1");

	auto Entity0 = m_Scene->GetEntity("entity0");
	auto Entity1 = m_Scene->GetEntity("entity1");

	//auto Trans0 = Entity0->AddComponent<TransformComponent>();
	//auto Trans1 = Entity1->AddComponent<TransformComponent>();

	//Trans0->SetTranslation(1, 1, 1);
	//Trans1->SetTranslation(2, 2, 2);

	m_Scene->Save(true);

	m_Scene->AddView<PlanarSceneView>();
	//m_View = std::make_shared<PlanarView>();
	//m_View->SetCamera(&m_Scene->GetMainCamera());
	//m_View->SetViewport(RHIViewport(m_Window->GetWidth(), m_Window->GetHeight()));
	//m_View->SetScissorRect(RHIScissorRect(m_Window->GetWidth(), m_Window->GetHeight()));

	//m_RenderGraph = RenderGraph::Create(GetRenderSettings(), *m_View);
}

void RenderTest::Render()
{
	//m_RenderGraph->Execute(*m_Scene);
}

REGISTER_APPLICATION(RenderTest);
