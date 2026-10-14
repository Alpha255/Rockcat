#pragma once

#include "Editor/ImGuiConfiguration.h"

class ImGuiEditor
{
public:
	ImGuiEditor(uint32_t AppWindowWidth, uint32_t AppWindowHeight);

	void Draw();

	/// Device pixels per logical pixel the UI should be laid out at, 1.0 at 96 DPI. Returns whether
	/// anything had to be rebuilt, which is the caller's cue to put the font atlas back on the GPU
	/// through VulkanImGuiRenderer::RebuildFontAtlas().
	bool SetDisplayScale(float DisplayScale);
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

	std::shared_ptr<ImGuiConfiguration> m_Configs;
	std::vector<std::shared_ptr<class ImGuiPanel>> m_Panels;
	std::vector<std::string> m_RecentScenes;
	bool m_ShowThemeEditor = false;
};