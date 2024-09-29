#include "Editor/Panels/LogConsole.h"
#include "Engine/Services/SpdLogService.h"
#include <Submodules/IconFontCppHeaders/IconsMaterialDesignIcons.h>

LogConsole::LogConsole()
	: ImGuiPanel("Log", ICON_MDI_COMMENT_TEXT_MULTIPLE)
{
	m_LogLevelConfigs.resize(spdlog::level::level_enum::off);
	m_LogLevelConfigs[spdlog::level::level_enum::trace] = 
	{
		ICON_MDI_MESSAGE_TEXT,
		Math::Color::Silver
	};
	m_LogLevelConfigs[spdlog::level::level_enum::debug] =
	{
		ICON_MDI_BUG,
		Math::Color::Cyan
	};
	m_LogLevelConfigs[spdlog::level::level_enum::info] = 
	{
		ICON_MDI_INFORMATION,
		Math::Color::White
	};
	m_LogLevelConfigs[spdlog::level::level_enum::warn] = 
	{
		ICON_MDI_ALERT,
		Math::Color::Yellow
	};
	m_LogLevelConfigs[spdlog::level::level_enum::err] = 
	{
		ICON_MDI_CLOSE_OCTAGON,
		Math::Color::Red
	};
	m_LogLevelConfigs[spdlog::level::level_enum::critical] = 
	{
		ICON_MDI_ALERT_OCTAGRAM,
		Math::Color::Purple
	};

	for (uint32_t Index = spdlog::level::level_enum::trace; Index < spdlog::level::level_enum::off; ++Index)
	{
		m_LogLevelFilter += 1 << Index;
	}
}

void LogConsole::DrawHeader()
{
	ImGui::AlignTextToFramePadding();
	
	IMGUI_SCOPED_COLOR(ImGuiCol_Button, Math::Color::Black);
	if (ImGui::Button(ICON_MDI_COG))
	{
		ImGui::OpenPopup("SettingsPopup");
	}
	if (ImGui::BeginPopup("SettingsPopup"))
	{
		if (ImGui::Button("Clear"))
		{
		}
		ImGui::EndPopup();
	}
	
	ImGui::SameLine();
	ImGui::TextUnformatted(ICON_MDI_MAGNIFY);
	ImGui::SameLine();

	auto& Style = ImGui::GetStyle();
	
	IMGUI_SCOPED_STYLE(ImGuiStyleVar_ItemSpacing, Math::Vector2(2.0f, Style.ItemSpacing.y));
	float LogLevelButtonWidth = ((ImGui::CalcTextSize(m_LogLevelConfigs[spdlog::level::debug].Icon) +
		Style.FramePadding * 2.0f).x + Style.ItemSpacing.x) * 6.0f;

	IMGUI_SCOPED_STYLE(ImGuiStyleVar_FrameBorderSize, 0.0f);
	IMGUI_SCOPED_COLOR(ImGuiCol_FrameBg, Math::Color::Black);
	m_TextFilter.Draw("###LogFilter", ImGui::GetContentRegionAvail().x - LogLevelButtonWidth);
	ImGui::DrawItemActivityOutline(2.0f, false);

	ImGui::SameLine();

	for (uint32_t Index = spdlog::level::level_enum::trace; Index < spdlog::level::level_enum::off; ++Index)
	{
		IMGUI_SCOPED_COLOR(ImGuiCol_Button, Math::Color::Black);
		ImGui::SameLine();

		uint32_t LogLevel = 1 << Index;
		const auto& TextColor = (LogLevel & m_LogLevelFilter) ? m_LogLevelConfigs[Index].TextColor : Math::Color::Gray;
		IMGUI_SCOPED_COLOR(ImGuiCol_Text, TextColor);

		if (ImGui::Button(m_LogLevelConfigs[Index].Icon))
		{
			m_LogLevelFilter ^= LogLevel;
		}

		if (ImGui::IsItemHovered())
		{
			ImGui::SetTooltip("%s", spdlog::level::level_names[Index]);
		}
	}

	if (!m_TextFilter.IsActive())
	{
		ImGui::SameLine();
		IMGUI_SCOPED_STYLE(ImGuiStyleVar_FramePadding, Math::Vector2(0.0f, Style.FramePadding.y));
		ImGui::TextUnformatted("Search...");
	}
}

void LogConsole::DrawTexts()
{
	if (ImGui::BeginTable("Logs", 2,
		ImGuiTableFlags_NoSavedSettings |
		ImGuiTableFlags_Borders |
		ImGuiTableFlags_SizingStretchProp |
		ImGuiTableFlags_ScrollY |
		ImGuiTableFlags_RowBg))
	{
		ImGui::TableSetupColumn("Level", ImGuiTableColumnFlags_NoSort | ImGuiTableColumnFlags_WidthFixed);
		ImGui::TableSetupColumn("Log", ImGuiTableColumnFlags_NoSort);
		ImGui::TableSetupScrollFreeze(0, 1);

		ImGui::TableHeadersRow();
		ImGui::TableNextRow();

		ImGui::EndTable();
	}
}

void LogConsole::OnDraw()
{
	DrawHeader();
	ImGui::Separator();
	DrawTexts();
}