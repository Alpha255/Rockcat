#include "Applications/RenderTest/RenderTest.h"
#include "Core/Main.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Asset/GlobalShaders/DefaultShading.h"
#include "Engine/Application/ApplicationConfigurations.h"
#include "Engine/Rendering/RenderGraph/ForwardRenderingPath.h"
#include "Engine/Services/RenderService.h"

void RenderTest::OnInitialize()
{
	{
		MaterialUnlit Material;
		Material.SetProjectionMatrix(Math::Matrix())
			.SetViewMatrix(Math::Matrix());
		Material.SetBaseColorMap("ToyCar\\glTF\\Fabric_baseColor.png");
	}
	
	m_Scene = Scene::Load<Scene>("RenderTest.scene");

	switch (GetGraphicsSettings().RenderingPath)
	{
	case ERenderingPath::ForwardRendering:
		m_RenderGraph = std::make_shared<ForwardRenderingPath>(GetRHI(), *m_Scene);
		break;
	case ERenderingPath::DeferredShading:
		assert(false);
		break;
	case ERenderingPath::DeferredLighting:
		assert(false);
		break;
	}
	m_RenderGraph->Setup();
}

void RenderTest::OnRenderFrame()
{
	m_RenderGraph->Execute();
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
