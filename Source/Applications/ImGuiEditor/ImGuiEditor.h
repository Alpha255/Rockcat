#pragma once

#include "Engine/Application/BaseApplication.h"

class ImGuiEditor final : public BaseApplication
{
public:
	using BaseApplication::BaseApplication;
protected:
	void OnInitialize() override final;
	void OnGUI(class Canvas&) override final {}
	void OnRenderFrame() override final;
private:
};