#pragma once

#include "Editor/ImGuiPanel.h"

class LogConsole : public ImGuiPanel
{
public:
	LogConsole();
protected:
	void OnDraw() override final;
private:
};
