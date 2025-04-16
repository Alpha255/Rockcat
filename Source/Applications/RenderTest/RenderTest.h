#pragma once

#include "Engine/Application/BaseApplication.h"

class RenderTest final : public BaseApplication
{
public:
	using BaseApplication::BaseApplication;

	void Initialize() override final;

	void RenderFrame() override final;
protected:
	void RenderGUI(class Canvas&) override final {}
private:
	std::shared_ptr<class Scene> m_Scene;
	std::shared_ptr<class RenderGraph> m_RenderGraph;
};