#include "Editor/Panels/LogConsole.h"
#include "Editor/Icons/IconsMaterialDesignIcons.h"
#include "Submodules/imgui/imgui.h"

LogConsole::LogConsole()
	: ImGuiPanel("Log", "")
{
}

void LogConsole::Draw()
{
	ImGui::AlignTextToFramePadding();
	if (ImGui::Button(ICON_LOG))
	{
		ImGui::OpenPopup("SettingPopup");
	}
	ImGui::SameLine();
	ImGui::TextUnformatted(ICON_MDI_MAGNIFY);
}