#include "Editor/Panels/LogConsole.h"
#include "Engine/Services/SpdLogService.h"
#include <Submodules/IconFontCppHeaders/IconsMaterialDesign.h>
#include <Submodules/IconFontCppHeaders/IconsMaterialDesignIcons.h>

LogConsole::LogConsole()
	: ImGuiPanel("Log", ICON_MDI_VIEW_LIST)
{
	m_LogLevelIcons.resize(spdlog::level::level_enum::off);
	m_LogLevelIcons[spdlog::level::level_enum::trace] = ICON_MDI_MESSAGE_TEXT;
	m_LogLevelIcons[spdlog::level::level_enum::debug] = ICON_MDI_BUG;
	m_LogLevelIcons[spdlog::level::level_enum::info] = ICON_MDI_INFORMATION;
	m_LogLevelIcons[spdlog::level::level_enum::warn] = ICON_MDI_ALERT;
	m_LogLevelIcons[spdlog::level::level_enum::err] = ICON_MDI_CLOSE_OCTAGON;
	m_LogLevelIcons[spdlog::level::level_enum::critical] = ICON_MDI_ALERT_OCTAGRAM;
}

void LogConsole::DrawHeader()
{
	ImGui::AlignTextToFramePadding();

	//IMGUI_SCOPED_FONT("MaterialIcons-Regular");
	
	IMGUI_SCOPED_COLOR(ImGuiCol_Button, Math::Color::Black);
	if (ImGui::Button(ICON_MDI_INFORMATION))
	{
	}
	//ImGui::SameLine();
	//ImGui::TextUnformatted(ICON_MD_ADD_ALERT);

	IMGUI_SCOPED_COLOR(ImGuiCol_Text, Math::Color::White);
	ImGui::SameLine();
	ImGui::TextUnformatted(ICON_MDI_INFORMATION);
	
	IMGUI_SCOPED_COLOR(ImGuiCol_Text, Math::Color::Yellow);
	ImGui::SameLine();
	ImGui::TextUnformatted(ICON_MDI_ALERT);

	IMGUI_SCOPED_COLOR(ImGuiCol_Text, Math::Color::Red);
	ImGui::SameLine();
	ImGui::TextUnformatted(ICON_MDI_CLOSE_OCTAGON);

	//auto& Style = ImGui::GetStyle();
	
	//IMGUI_SCOPED_STYLE(ImGuiStyleVar_ItemSpacing, Math::Vector2(2.0f, Style.ItemSpacing.y));
	//float LogLevelButtonWidth = ((ImGui::CalcTextSize(m_LogLevelIcons[spdlog::level::debug]) + 
	//	Style.FramePadding * 2.0f).x + Style.ItemSpacing.x) * 6.0f;

	//IMGUI_SCOPED_STYLE(ImGuiStyleVar_FrameBorderSize, 0.0f);
	//IMGUI_SCOPED_COLOR(ImGuiCol_FrameBg, Math::Color::White);
	//m_TextFilter.Draw("###ConsoleFilter", ImGui::GetContentRegionAvail().x - LogLevelButtonWidth);
}

void LogConsole::OnDraw()
{
	DrawHeader();

	IMGUI_SCOPED_FONT("Roboto-Black");
	ImGui::Text("The quick brown fox jumps over the lazy dog.");
}