#include "Editor/Panels/LogConsole.h"
#include "Editor/Icons/IconsMaterialDesignIcons.h"

LogConsole::LogConsole()
	: ImGuiPanel("Log")
{
}

void LogConsole::OnDraw()
{
	IMGUI_SCOPED_FONT("Roboto-Black");

	ImGui::AlignTextToFramePadding();
	if (ImGui::Button(ICON_LOG))
	{
		ImGui::OpenPopup("SettingPopup");
	}
	ImGui::SameLine();
	ImGui::TextUnformatted(ICON_MDI_MAGNIFY);
}