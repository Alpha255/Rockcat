#pragma once

#include "Editor/ImGuiPanel.h"

class AssetBrowserPanel : public ImGuiPanel
{
public:
	AssetBrowserPanel();
protected:
	void OnDraw() override final;
private:
};