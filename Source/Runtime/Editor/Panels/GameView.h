#pragma once

#include "Editor/ImGuiPanel.h"

class GameView : public ImGuiPanel
{
public:
	GameView();
protected:
	void OnDraw() override final;
private:
};