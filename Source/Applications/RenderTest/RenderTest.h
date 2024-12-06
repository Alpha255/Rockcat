#pragma once

#include "Engine/Application/BaseApplication.h"

class RenderTest final : public BaseApplication
{
public:
	using BaseApplication::BaseApplication;
protected:
	void Tick(float ElapsedSeconds) override final;

	void OnInitialize() override final;
	void OnRenderGUI(class Canvas&) override final {}
	void OnRenderFrame() override final;
private:
	std::shared_ptr<class Scene> m_Scene;
	std::shared_ptr<class RenderGraph> m_RenderGraph;
};