#pragma once

#include "Editor/ImGuiConfigurations.h"

class ImGuiEditor
{
public:
	ImGuiEditor(uint32_t AppWindowWidth, uint32_t AppWindowHeight);

	void Draw();
protected:
private:
	struct ScopedDockSpace
	{
		ScopedDockSpace(const char* const DockSpaceName);
		~ScopedDockSpace();

		void RebuildDefaultLayout() {}

		bool Open;
		ImGuiDockNodeFlags DockNodeFlags;
		ImGuiWindowFlags WindowFlags;
		const char* const Name;
	};

	void DrawMenuBar();

	std::shared_ptr<ImGuiConfigurations> m_Configs;
	std::vector<std::shared_ptr<class ImGuiPanel>> m_Panels;
	std::vector<std::string> m_RecentScenes;
	bool m_ShowThemeEditor = false;
};