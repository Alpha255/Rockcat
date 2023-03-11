#pragma once

#include "Runtime/Application.h"

class RenderTest final : public Application
{
public:
	using Application::Application;

	void OnStartup() override final;

	void RenderGUI() override final;
protected:
private:
};