#include "Editor/Panels/LogConsolePanel.h"
#include "Editor/Icons/Icons.h"

LogConsolePanel::LogMessage::LogMessage(const spdlog::details::log_msg& Message, int32_t MessageID)
	: Level(Message.level)
	, Text(Message.payload.data())
	, Time(Message.time)
	, ID(MessageID)
{
}

LogConsolePanel::LogConsolePanel()
	: ImGuiPanel("Log", ICON_LOG_CONSOLE)
{
	m_LogLevelConfigs.resize(spdlog::level::level_enum::off);
	m_LogLevelConfigs[spdlog::level::level_enum::trace] = 
	{
		ICON_LOG_TRACE,
		Math::Color::Silver
	};
	m_LogLevelConfigs[spdlog::level::level_enum::debug] =
	{
		ICON_LOG_DEBUG,
		Math::Color::Cyan
	};
	m_LogLevelConfigs[spdlog::level::level_enum::info] = 
	{
		ICON_LOG_INFO,
		Math::Color::White
	};
	m_LogLevelConfigs[spdlog::level::level_enum::warn] = 
	{
		ICON_LOG_WARNING,
		Math::Color::Yellow
	};
	m_LogLevelConfigs[spdlog::level::level_enum::err] = 
	{
		ICON_LOG_ERROR,
		Math::Color::Red
	};
	m_LogLevelConfigs[spdlog::level::level_enum::critical] = 
	{
		ICON_LOG_FATAL,
		Math::Color::Purple
	};

	for (uint32_t Index = spdlog::level::level_enum::trace; Index < spdlog::level::level_enum::off; ++Index)
	{
		m_LogLevelFilter |= 1 << Index;
	}
}

void LogConsolePanel::Log(const SpdLogMessage& Log)
{
	/// TODO: Thread safe ???
	m_LogMessages.emplace_back(LogMessage(Log, static_cast<int32_t>(m_LogMessages.size())));
}

void LogConsolePanel::DrawHeader()
{
	ImGui::AlignTextToFramePadding();
	
	IMGUI_SCOPED_COLOR(ImGuiCol_Button, Math::Color::Black);
	if (ImGui::Button(ICON_LOG_SETTING))
	{
		ImGui::OpenPopup("SettingsPopup");
	}
	if (ImGui::BeginPopup("SettingsPopup"))
	{
		if (ImGui::Button("Clear"))
		{
			m_LogMessages.clear();
		}
		ImGui::EndPopup();
	}
	
	ImGui::SameLine();
	ImGui::TextUnformatted(ICON_LOG_SEARCH);

	auto& Style = ImGui::GetStyle();
	
	IMGUI_SCOPED_STYLE(ImGuiStyleVar_ItemSpacing, Math::Vector2(2.0f, Style.ItemSpacing.y));
	float LogLevelButtonWidth = ((ImGui::CalcTextSize(m_LogLevelConfigs[spdlog::level::debug].Icon) +
		Style.FramePadding * 2.0f).x + Style.ItemSpacing.x) * 6.0f;

	IMGUI_SCOPED_STYLE(ImGuiStyleVar_FrameBorderSize, 0.0f);
	IMGUI_SCOPED_COLOR(ImGuiCol_FrameBg, Math::Color::Black);
	ImGui::SameLine();
	const float FilterWidth = (ImGui::GetContentRegionAvail().x - LogLevelButtonWidth) / 2.0f;
	m_TextFilter.Draw("###LogFilter", FilterWidth);
	ImGui::DrawItemActivityOutline(2.0f, true);

	ImGui::SameLine(0.0f, FilterWidth);
	for (uint32_t Index = spdlog::level::level_enum::trace; Index < spdlog::level::level_enum::off; ++Index)
	{
		IMGUI_SCOPED_COLOR(ImGuiCol_Button, Math::Color::Black);

		uint32_t LogLevel = 1 << Index;
		const auto& TextColor = (LogLevel & m_LogLevelFilter) ? m_LogLevelConfigs[Index].TextColor : Math::Color::Gray;
		IMGUI_SCOPED_COLOR(ImGuiCol_Text, TextColor);

		if (ImGui::Button(m_LogLevelConfigs[Index].Icon))
		{
			m_LogLevelFilter ^= LogLevel;
		}

		if (ImGui::IsItemHovered())
		{
			ImGui::SetTooltip("%s", spdlog::level::level_string_views[Index]);
		}

		ImGui::SameLine();
	}

	if (!m_TextFilter.IsActive())
	{
		ImGui::SameLine();
		IMGUI_SCOPED_STYLE(ImGuiStyleVar_FramePadding, Math::Vector2(0.0f, Style.FramePadding.y));
		IMGUI_SCOPED_COLOR(ImGuiCol_Text, Math::Color::Gray);
		ImGui::SetCursorPosX(ImGui::GetFontSize() * 4.0f);
		ImGui::TextUnformatted("Search...");
	}
}

void LogConsolePanel::DrawLogs()
{
	if (ImGui::BeginTable("Logs", 2,
		ImGuiTableFlags_NoSavedSettings |
		ImGuiTableFlags_Borders |
		ImGuiTableFlags_SizingStretchProp |
		ImGuiTableFlags_ScrollY |
		ImGuiTableFlags_RowBg))
	{
		ImGui::TableSetupScrollFreeze(0, 1);

		for (const auto& Message : m_LogMessages)
		{
			if (m_TextFilter.IsActive() && !m_TextFilter.PassFilter(Message.Text.c_str()))
			{
				continue;
			}

			if (!(m_LogLevelFilter & (1 << Message.Level)))
			{
				continue;
			}

			IMGUI_SCOPED_COLOR(ImGuiCol_Text, m_LogLevelConfigs[Message.Level].TextColor);
			ImGui::TableNextColumn();
			ImGui::TextUnformatted(m_LogLevelConfigs[Message.Level].Icon);

			IMGUI_SCOPED_ID(Message.ID);
			ImGui::TableNextColumn();
			ImGui::TextUnformatted(Message.Text.c_str());

			IMGUI_SCOPED_COLOR(ImGuiCol_Text, Math::Color::White);
			if (ImGui::BeginPopupContextItem(Message.Text.c_str()))
			{
				if (ImGui::MenuItem("Copy"))
				{
					ImGui::SetClipboardText(Message.Text.c_str());
				}

				ImGui::EndPopup();
			}

			ImGui::TableNextRow();
		}

		ImGui::EndTable();
	}
}

void LogConsolePanel::OnDraw()
{
	DrawHeader();
	ImGui::Separator();
	DrawLogs();
}
