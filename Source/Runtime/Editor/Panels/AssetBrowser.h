#pragma once

#include "Editor/ImGuiPanel.h"

class AssetBrowser : public ImGuiPanel
{
public:
	AssetBrowser();
protected:
	void OnDraw() override final;
private:
};