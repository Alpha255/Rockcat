#pragma once

#include "Editor/ImGuiPanel.h"

class LogConsole : public ImGuiPanel
{
public:
	LogConsole();
protected:
	void OnDraw() override final;
private:
	void DrawHeader();
	void DrawTexts();

	struct LogLevelConfig
	{
		const char* Icon = nullptr;
		Math::Color TextColor = Math::Color::White;
	};

	uint32_t m_LogLevelFilter = 0u;
	std::vector<LogLevelConfig> m_LogLevelConfigs;
	ImGuiTextFilter m_TextFilter;
};
