#include "MultithreadedRendering.h"
#include "Colorful/Public/ImGUI.h"

static std::mutex s_Mutex;

void MultithreadedRendering::RenderThreadFunc(Scene::eDrawType drawType)
{
	assert(drawType < Scene::eTypeCount);

	m_Scene.DrawByPart(m_Camera, m_WindowSize.first, m_WindowSize.second, drawType, m_Context[drawType]);

	m_Context[drawType].FinishCommandList(true, m_CommandList[drawType]);
}

void MultithreadedRendering::PrepareScene()
{
	m_Scene.Initialize();

	AutoFocus(m_Scene.SquidRoom, 0.3f);

	TaskScheduler::Instance().Initialize(Scene::eTypeCount);

	for (uint32_t i = 0U; i < Scene::eTypeCount; ++i)
	{
		m_Context[i].CreateAsDeferredContext();
	}
}

void MultithreadedRendering::RenderScene()
{
	if (m_RenderingMode == eSingelThread)
	{
		m_Scene.Draw(m_Camera, m_WindowSize.first, m_WindowSize.second);
	}
	else if (m_RenderingMode == eMultiThreadByScene)
	{
		for (uint32_t i = 0U; i < Scene::eTypeCount; ++i)
		{
			TaskScheduler::Instance().AddTask([=] {
				RenderThreadFunc((Scene::eDrawType)i);
			});
		}

		TaskScheduler::Instance().WaitUntilDone();

		for (uint32_t i = 0U; i < Scene::eTypeCount; ++i)
		{
			REngine::Instance().ExecuteCommandList(true, m_CommandList[i]);
			m_CommandList[i].Release();
		}

		REngine::Instance().ResetDefaultRenderSurfaces(Color::DarkBlue, nullptr, false);
	}

	ImGui::Text("FPS: %.2f", m_FPS);
	ImGui::Combo("RenderingMode", &m_RenderingMode, "SingleThread\0Multithread");
}
