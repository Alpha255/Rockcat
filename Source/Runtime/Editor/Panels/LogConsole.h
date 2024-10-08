#pragma once

#include "Editor/ImGuiPanel.h"

class LogConsole : public ImGuiPanel, public spdlog::sinks::sink
{
public:
	LogConsole();

	void log(const spdlog::details::log_msg& Message) override;

	void flush() override {}
protected:
	void OnDraw() override final;
private:
	struct LogMessage
	{
		LogMessage(const spdlog::details::log_msg& Message, int32_t MessageID);

		spdlog::level::level_enum Level;
		std::string Text;
		std::chrono::system_clock::time_point Time;
		int32_t ID;
	};

	void DrawHeader();
	void DrawTexts();

	struct LogLevelConfig
	{
		const char* Icon = nullptr;
		Math::Color TextColor = Math::Color::White;
	};

	uint32_t m_LogLevelFilter = 0u;
	std::vector<LogLevelConfig> m_LogLevelConfigs;
	std::vector<LogMessage> m_LogMessages;
	ImGuiTextFilter m_TextFilter;
};
