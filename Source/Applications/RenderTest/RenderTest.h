#pragma once

#include "Application/BaseApplication.h"

class RenderTest final : public BaseApplication
{
public:
	using BaseApplication::BaseApplication;

	void Initialize() override final;
	void Render(class RHITexture* RenderSurface) override final;
protected:
	void RenderGUI() override final {}
private:
	std::shared_ptr<class Scene> m_Scene;
	std::shared_ptr<class RenderGraph> m_RenderGraph;
	std::shared_ptr<class IView> m_View;
};