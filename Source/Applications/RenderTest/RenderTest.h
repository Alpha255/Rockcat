#pragma once

#include "Runtime/Engine/Application/BaseApplication.h"

class RenderTest final : public BaseApplication
{
public:
	using BaseApplication::BaseApplication;

	void OnStartup() override final;

	void OnRender() override final;
protected:
private:
	std::unique_ptr<class Scene> m_Scene;
};