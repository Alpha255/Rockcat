#pragma once

#include "Engine/Application/BaseApplication.h"

class RenderTest final : public BaseApplication
{
public:
	using BaseApplication::BaseApplication;

	void RenderFrame() override final;
protected:
	void InitializeImpl() override final;
	void RenderGUI(class Canvas&) override final {}
private:
	std::shared_ptr<class Scene> m_Scene;
	std::shared_ptr<class RenderGraph> m_RenderGraph;
	std::shared_ptr<class IView> m_View;
};