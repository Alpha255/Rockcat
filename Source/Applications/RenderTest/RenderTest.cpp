#include "Applications/RenderTest/RenderTest.h"
#include "Core/Main.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Application/ApplicationConfiguration.h"
#include "Engine/Rendering/RenderGraph/ForwardRenderingPath.h"
#include "Engine/Services/RenderService.h"
#include "Engine/Services/TaskFlowService.h"
#include "Engine/Asset/GlobalShaders.h"

void RenderTest::Initialize()
{
	m_Scene = Scene::Load<Scene>("RenderTest.scene");

	auto& Window = GetWindow();
	auto& Cameras = m_Scene->GetCameras();
	m_RenderGraph = RenderGraph::Create(GetGraphicsSettings());
}

void RenderTest::Tick(float ElapsedSeconds)
{
	BaseApplication::Tick(ElapsedSeconds);

	m_Scene->Update();
}

void RenderTest::RenderFrame()
{
	m_RenderGraph->Execute(*m_Scene);
}

REGISTER_APPLICATION(RenderTest, "RenderTest.json")
