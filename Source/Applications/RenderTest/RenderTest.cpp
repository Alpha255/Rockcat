#include "Applications/RenderTest/RenderTest.h"
#include "Core/Main.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Rendering/RenderGraph/RenderGraph.h"
#include "Engine/Services/TaskFlowService.h"

void RenderTest::Initialize()
{
	struct Test
	{
	};
	std::set<std::shared_ptr<Test>> Tests;
	std::mutex Lock;

	std::vector<uint32_t> Counter(1024u);
	tf::ParallelFor(Counter.begin(), Counter.end(), [&Lock, &Tests](const uint32_t&) {
		thread_local std::shared_ptr<Test> ThreadTest;
		if (!ThreadTest)
		{
			ThreadTest = std::make_shared<Test>();
			
			std::lock_guard Locker(Lock);
			Tests.emplace(ThreadTest);
		}
	})->Wait();

	m_Scene = Scene::Load<Scene>("RenderTest.scene");
	m_RenderGraph = RenderGraph::Create(GetRenderBackend(), GetRenderSettings(), GetRenderViewport());
}

void RenderTest::Tick(float ElapsedSeconds)
{
	m_Scene->Tick(ElapsedSeconds);
}

void RenderTest::RenderFrame()
{
	m_RenderGraph->Execute(*m_Scene);
}

REGISTER_APPLICATION(RenderTest, "RenderTest.json")
