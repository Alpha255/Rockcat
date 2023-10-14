#pragma once

#include "Runtime/Engine/Application/BaseApplication.h"

class RenderTest final : public BaseApplication
{
public:
	using BaseApplication::BaseApplication;
protected:
	void OnInitialize() override final;
	void OnGUI(class Canvas&) override final {}
private:
	std::shared_ptr<class Scene> m_Scene;
};