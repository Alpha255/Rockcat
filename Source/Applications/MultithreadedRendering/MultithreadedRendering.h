#pragma once

#include "Public/IRenderApplication.h"
#include "Public/Model.h"
#include "Scene.h"
#include "Base/TaskScheduler.h"

class MultithreadedRendering : public IRenderApplication
{
public:
	MultithreadedRendering() = default;
	~MultithreadedRendering() = default;

	void PrepareScene() override;
	void RenderScene() override;
	inline void UpdateScene(float elapsedTime, float totalTime) override
	{
		m_Scene.Update(elapsedTime, totalTime);
	}

	void RenderThreadFunc(Scene::eDrawType drawType);

	enum eRenderingMode
	{
		eSingelThread,          /// Traditional rendering, one thread, immediate device context
		eMultiThreadByScene,    /// Multiple threads, one per scene, each with one deferred device context 
	};

protected:
private:
	int32_t m_RenderingMode = eSingelThread;

	Scene m_Scene;

	RCommandList m_CommandList[Scene::eTypeCount];
	RContext m_Context[Scene::eTypeCount];
};
