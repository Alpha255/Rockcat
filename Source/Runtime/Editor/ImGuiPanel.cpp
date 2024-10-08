#include "Editor/ImGuiPanel.h"
#include <Submodules/imgui/imgui.h>

ImGuiPanel::ImGuiPanel(const char* Title, const char* Icon, const ImGuiWidget* Parent, int32_t WindowsFlags)
	: ImGuiWidget(Title, Parent)
	, m_WindowFlags(WindowsFlags | ImGuiWindowFlags_NoCollapse)
	, m_Title(Title)
	, m_TitleWithIcon(StringUtils::Format("%s%s%s###%s", Icon ? Icon : "", Icon ? " " : "", Title, StringUtils::Lowercase(Title).c_str()))
{
}

void ImGuiPanel::Draw()
{
	if (ImGui::Begin(m_TitleWithIcon.c_str(), &m_Show, m_WindowFlags))
	{
		OnDraw();
	}
	ImGui::End();
}
