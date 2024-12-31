#include "Applications/RenderTest/RenderTest.h"
#include "Core/Main.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Application/ApplicationConfigurations.h"
#include "Engine/Rendering/RenderGraph/ForwardRenderingPath.h"
#include "Engine/Services/RenderService.h"
#include "Engine/Services/TaskFlowService.h"

void RenderTest::OnInitialize()
{	
	m_Scene = Scene::Load<Scene>("RenderTest.scene");

	auto& Window = GetWindow();
	auto& Cameras = m_Scene->GetCameras();
	m_RenderGraph = RenderGraph::Create(GetGraphicsSettings());

	std::vector<uint32_t> Numbers(1024u);
	struct TestTask : public Task
	{
		using Task::Task;

		TestTask(uint32_t Index, std::vector<uint32_t>& Numbers)
			: Task("TestTask")
			, MyIndex(Index)
			, MyNumbers(Numbers)
		{
		}

		void Execute() override final
		{
			MyNumbers[MyIndex] = MyIndex;
		}

		uint32_t MyIndex;
		std::vector<uint32_t>& MyNumbers;
	};

	TaskFlow Flow;
	for (uint32_t Index = 0u; Index < Numbers.size(); ++Index)
	{
		Flow.Emplace<TestTask>(Index, Numbers);
	}
	tf::DispatchTaskFlow_WaitDone(Flow);

	LOG_INFO("Test");
}

void RenderTest::Tick(float ElapsedSeconds)
{
	BaseApplication::Tick(ElapsedSeconds);

	m_Scene->Update();
}

void RenderTest::OnRenderFrame()
{
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
