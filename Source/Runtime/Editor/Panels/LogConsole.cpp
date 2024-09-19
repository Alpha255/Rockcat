#include "Editor/Panels/LogConsole.h"
#include "Engine/Services/SpdLogService.h"
#include <Submodules/IconFontCppHeaders/IconsMaterialDesign.h>

LogConsole::LogConsole()
	: ImGuiPanel("Log")
{
	m_LogLevelIcons.resize(spdlog::level::level_enum::off);
	m_LogLevelIcons[spdlog::level::level_enum::trace] = ICON_MD_MESSAGE;
	m_LogLevelIcons[spdlog::level::level_enum::debug] = ICON_MD_BUG_REPORT;
	m_LogLevelIcons[spdlog::level::level_enum::info] = ICON_MD_INFO;
	m_LogLevelIcons[spdlog::level::level_enum::warn] = ICON_MD_WARNING;
	m_LogLevelIcons[spdlog::level::level_enum::err] = ICON_MD_ERROR;
	m_LogLevelIcons[spdlog::level::level_enum::critical] = ICON_MD_HEART_BROKEN;
}

void LogConsole::DrawHeader()
{
	ImGui::AlignTextToFramePadding();

	IMGUI_SCOPED_FONT("MaterialIcons-Regular");
	
	IMGUI_SCOPED_COLOR(ImGuiCol_Button, Math::Color::Black);
	if (ImGui::Button(ICON_MD_MESSAGE))
	{

	}
	ImGui::SameLine();
	ImGui::TextUnformatted(ICON_MD_ADD_ALERT);

	IMGUI_SCOPED_COLOR(ImGuiCol_Text, Math::Color::White);
	ImGui::SameLine();
	ImGui::TextUnformatted(ICON_MD_INFO);
	
	IMGUI_SCOPED_COLOR(ImGuiCol_Text, Math::Color::Yellow);
	ImGui::SameLine();
	ImGui::TextUnformatted(ICON_MD_WARNING);

	IMGUI_SCOPED_COLOR(ImGuiCol_Text, Math::Color::Red);
	ImGui::SameLine();
	ImGui::TextUnformatted(ICON_MD_ERROR_OUTLINE);

	IMGUI_SCOPED_COLOR(ImGuiCol_Text, Math::Color::Red);
	ImGui::SameLine();
	ImGui::TextUnformatted(ICON_MD_HEART_BROKEN);

	auto& Style = ImGui::GetStyle();
	
	float OldSpacing = Style.ItemSpacing.x;
	Style.ItemSpacing.x = 2.0f;
	float LogLevelButtonWidth = ((ImGui::CalcTextSize(m_LogLevelIcons[spdlog::level::debug]) + 
		Style.FramePadding * 2.0f).x + Style.ItemSpacing.x) * 6.0f;

	IMGUI_SCOPED_STYLE(ImGuiStyleVar_FrameBorderSize, 0.0f);
	IMGUI_SCOPED_COLOR(ImGuiCol_FrameBg, Math::Color::White);
	m_TextFilter.Draw("###ConsoleFilter", ImGui::GetContentRegionAvail().x - LogLevelButtonWidth);
}

void LogConsole::OnDraw()
{
	DrawHeader();

	IMGUI_SCOPED_FONT("Roboto-Black");
	ImGui::Text("The quick brown fox jumps over the lazy dog.");
}