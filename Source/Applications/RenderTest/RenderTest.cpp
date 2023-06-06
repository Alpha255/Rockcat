#include "Applications/RenderTest/RenderTest.h"
#include "Runtime/Engine/Scene/Scene.h"
#include "Runtime/Engine/Asset/SceneAsset.h"
#include "Runtime/Core/Main.h"
//#include <Submodules/imgui/imgui.h>

void RenderTest::OnStartup()
{
#if 0
	m_Scene = Scene::Load("RenderTest.json");
	m_Camera = m_Scene->MainCamera();
	//m_Camera->onWindowResized(1280, 720);
	m_Camera->SetPerspective(Math::PI_Div4, 1280.0f / 720.0f, 0.1f, 500.0f);
	m_Camera->SetLookAt(Vector3(0.0f, 0.0f, -5.0f), Vector3(0.0f, 0.0f, 0.0f));

	m_Camera->SetSpeed(0.01f, 15.0f);

	m_Camera->SetMode(Camera::EMode::FirstPerson);
#if 0
	m_Scene->AddModel("outdoor_columns.obj", nullptr);
	m_Scene->Save(true);
#endif
#endif

	m_Scene = std::make_unique<Scene>("RenderTest.scene");
}

void RenderTest::OnRender()
{
#if 0
	ImGui::Begin("Settings");

	ImGui::Text("FrameTime %.2fms", RHI::Profiler::Stats::Get().AverageFrameTime());

	ImGui::Text("FPS %.2f", RHI::Profiler::Stats::Get().FPS());

	ImGui::ShowDemoWindow();
	
	ImGui::End();
#endif
}

REGISTER_APPLICATION(RenderTest, "RenderTest.json")
