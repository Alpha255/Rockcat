#include "Applications/RenderTest/RenderTest.h"
#include "Core/Main.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Rendering/RenderGraph/RenderGraph.h"

void RenderTest::Initialize()
{
	m_Scene = Scene::Load<Scene>("RenderTest.scene");
	m_RenderGraph = RenderGraph::Create(GetRenderBackend(), GetRenderSettings());
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
