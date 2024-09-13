#include "Editor/ImGuiPanel.h"
#include <Submodules/imgui/imgui.h>

ImGuiPanel::ImGuiPanel(const char* Title, const char* Icon, const ImGuiWidget* Parent)
	: ImGuiWidget(Title, Parent)
	, m_Title(Title)
	, m_WindowFlags(ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse)
{
	if (Icon)
	{
	}
}

void ImGuiPanel::OnRender()
{
	if (ImGui::Begin(m_Title.data(), &m_Show, m_WindowFlags))
	{
		Draw();
		ImGui::End();
	}
}
