#include "Applications/RenderTest/RenderTest.h"
#include "Core/Main.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Asset/GlobalShaders/DefaultShading.h"
#include "Engine/Application/ApplicationConfigurations.h"
#include "Engine/Rendering/RenderGraph/ForwardRenderingPath.h"
#include "Engine/Services/RenderService.h"

void RenderTest::OnInitialize()
{	
	m_Scene = Scene::Load<Scene>("RenderTest.scene");
	m_RenderGraph = RenderGraph::Create(GetGraphicsSettings());
}

void RenderTest::OnRenderFrame()
{
	m_Scene->Update();

	m_RenderGraph->Execute(*m_Scene);
}

#if 0
void RenderTest::OnRenderGUI()
{
	ImGui::Begin("Settings");

	ImGui::Text("FrameTime %.2fms", RHI::Profiler::Stats::Get().AverageFrameTime());

	ImGui::Text("FPS %.2f", RHI::Profiler::Stats::Get().FPS());

	ImGui::ShowDemoWindow();

	ImGui::End();
}
#endif

REGISTER_APPLICATION(RenderTest, "RenderTest.json")
