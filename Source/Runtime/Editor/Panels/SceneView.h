#pragma once

#include "Editor/ImGuiPanel.h"

class SceneView : public ImGuiPanel
{
public:
	SceneView();
protected:
	void OnDraw() override final;
private:
};