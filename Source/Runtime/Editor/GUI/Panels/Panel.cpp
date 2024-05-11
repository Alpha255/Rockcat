#include "Editor/GUI/Panels/Panel.h"
#include <Submodules/imgui/imgui.h>

Panel::Panel(const char* Title, const char* Icon)
	: m_Title(Title)
	, m_WindowFlags(ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse)
{
	if (Icon)
	{
	}
}

void Panel::Draw()
{
	if (m_Show && Begin())
	{
		OnGUI();
		End();
	}
}

void Panel::Collapse()
{
}

void Panel::Minimize()
{
}

void Panel::Maximize()
{
}

bool Panel::Begin()
{
	ImGuiWindowFlags Flags = m_WindowFlags;

	if (IsLocked())
	{
		Flags |= ImGuiWindowFlags_NoMove;
	}

	auto CanBegin = ImGui::Begin(m_Title.data(), &m_Show, Flags);
	m_Pos = Math::Vector2(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y);
	m_Size = Math::Vector2(ImGui::GetWindowSize().x, ImGui::GetWindowSize().y);

	if (!ShouldDraw())
	{
		ImGui::End();
		return false;
	}

	return CanBegin;
}

void Panel::End()
{
	if (ShouldDraw())
	{
		ImGui::End();
	}
}

bool Panel::ShouldDraw() const
{
	return ImGui::IsWindowDocked() || !IsCollapsed();
}
