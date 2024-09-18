#include "Editor/ImGuiPanel.h"
#include <Submodules/imgui/imgui.h>

ImGuiPanel::ImGuiPanel(const char* Title, const ImGuiWidget* Parent, int32_t WindowsFlags)
	: ImGuiWidget(Title, Parent)
	, m_Title(Title)
	, m_WindowFlags(WindowsFlags)
{
}

void ImGuiPanel::Draw()
{
	if (ImGui::Begin(m_Title.data(), &m_Show, m_WindowFlags))
	{
		OnDraw();
		ImGui::End();
	}
}
