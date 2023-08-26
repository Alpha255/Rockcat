#pragma once

#include "Runtime/Engine/Application/BaseApplication.h"

class RenderTest final : public BaseApplication
{
public:
	using BaseApplication::BaseApplication;

	void OnStartup() override final;

	void OnRenderScene() override final;

	void OnRenderGUI() override final;
protected:
private:
	std::shared_ptr<class Scene> m_Scene;
};