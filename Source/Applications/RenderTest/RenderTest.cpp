#include "Applications/RenderTest/RenderTest.h"
#include "Application/ApplicationManager.h"
#include "Scene/Scene.h"
#include "Scene/SceneView.h"
#include "Rendering/RenderGraph/RenderGraph.h"

#include "Asset/GlobalShaders.h"
#include "Scene/Components/TransformComponent.h"

#include "Asset/ShaderPermutation.h"
#include "Core/Name.h"
#include "Async/Task.h"
#include "Scene/Components/CameraComponent.h"
#include "Scene/Components/StaticMeshComponent.h"
#include "Rendering/SceneRenders/SceneRenderer.h"
#include "Rendering/RenderGraph/RenderGraph.h"
#include "Application/ApplicationSettings.h"

class TestPemutation1 : public ShaderDefineInt32<0, 1, 2, 3>
{
};

class TestPemutation2 : public ShaderDefineBool
{
};

class Permutation : public ShaderVariants<TestPemutation1, TestPemutation2>
{
};

void RenderTest::Initialize()
{
	auto TaskA = std::make_shared<TFTask>("TaskA", []() {
		LOG_INFO(LogDefault, "TaskA");
	});

	auto TaskB = TFTask::Launch("TaskB", []() {
		LOG_INFO(LogDefault, "TaskB");
	}, {TaskA.get()});

	auto TaskC = std::make_shared<TFTask>("TaskC", []() {
		LOG_INFO(LogDefault, "TaskC");
	});

	auto TaskD = std::make_shared<TFTask>("TaskD", []() {
		LOG_INFO(LogDefault, "TaskD");
	}, TFTask::EThread::WorkerThread, TFTask::EPriority::High);

	TaskC->AddPrerequisite(*TaskB);
	TaskC->AddPrerequisite(*TaskD);
	TaskC->Trigger();
	TaskD->Trigger();

	//TaskA->Wait();
	//TaskB->Wait();
	//TaskC->Wait();
	//TaskD->Wait();

	auto GetState = [](std::shared_ptr<TFTask>& Task) {
		return Task->IsCompleted() ? "Completed" : "Uncompleted";
	};

	LOG_INFO(LogDefault, "TaskA is {}, TaskB is {}, TaskC is {}, TaskD is {}", GetState(TaskA), GetState(TaskB), GetState(TaskC), GetState(TaskD));
	TaskD->Wait();

	size_t NumVariants = Permutation::Num;

	auto Comp = new CameraComponent();
	const auto IsACamera = Comp->IsA<CameraComponent>();
	const auto IsABase = Comp->IsA<ComponentBase>();
	const auto IsATransform = Comp->IsA<TransformComponent>();

	auto StaticMesh = new StaticMeshComponent();
	const auto IsAStaticMesh = StaticMesh->IsA<StaticMeshComponent>();
	const auto IsAPrimitive = StaticMesh->IsA<PrimitiveComponent>();
	const auto IsATrans = StaticMesh->IsA<TransformComponent>();
	const auto IsACam = StaticMesh->IsA<CameraComponent>();

	m_Scene = Scene::Load(Paths::ScenePath() / "RenderTest.json");

	//m_Scene->AddEntity(EntityID::NullIndex, "entity0");
	//m_Scene->AddEntity(EntityID::NullIndex, "entity1");

	//auto Entity0 = m_Scene->GetEntity("entity0");
	//auto Entity1 = m_Scene->GetEntity("entity1");

	//auto Trans0 = Entity0->AddComponent<TransformComponent>();
	//auto Trans1 = Entity1->AddComponent<TransformComponent>();

	//Trans0->SetTranslation(1, 1, 1);
	//Trans1->SetTranslation(2, 2, 2);

	//m_Scene->Save(true);

	//m_Scene->AddView<PlanarSceneView>();
	//m_View = std::make_shared<PlanarView>();
	//m_View->SetCamera(&m_Scene->GetMainCamera());
	//m_View->SetViewport(RHIViewport(m_Window->GetWidth(), m_Window->GetHeight()));
	//m_View->SetScissorRect(RHIScissorRect(m_Window->GetWidth(), m_Window->GetHeight()));

	//m_RenderGraph = RenderGraph::Create(GetRenderSettings(), *m_View);
}

void RenderTest::Render()
{
	RDGRenderGraph RenderGraph(m_Settings->GetRenderSettings());
	std::unique_ptr<SceneRenderer> Renderer = SceneRenderer::Create(m_Settings->GetRenderSettings().RenderingPath, *m_Scene);

	Renderer->Render(RenderGraph);
}

REGISTER_APPLICATION(RenderTest);
