#pragma once

#include "Editor/ImGuiConfigurations.h"

class ImGuiEditor
{
public:
	ImGuiEditor(uint32_t AppWindowWidth, uint32_t AppWindowHeight);

	void Draw();
protected:
private:
	void DrawMenuBar();

	std::shared_ptr<ImGuiConfigurations> m_Configs;
	std::vector<std::shared_ptr<class ImGuiPanel>> m_Panels;
	bool m_ShowThemeEditor = false;
};