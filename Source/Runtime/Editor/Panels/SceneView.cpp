#include "Editor/Panels/SceneView.h"
#include <Submodules/IconFontCppHeaders/IconsMaterialDesignIcons.h>

SceneView::SceneView()
	: ImGuiPanel(
		"Scene", 
		ICON_MDI_EYE,
		nullptr,
		ImGuiWindowFlags_NoCollapse | 
		ImGuiWindowFlags_NoScrollbar | 
		ImGuiWindowFlags_NoScrollWithMouse)
{
}

void SceneView::OnDraw()
{
	DrawToolBar();
}

void SceneView::DrawToolBar()
{
	ImGui::Indent();
	IMGUI_SCOPED_COLOR(ImGuiCol_Button, Math::Color::Black);

	DrawToolBarButton(ImGuiGizmo::EMode::Selection, ICON_MDI_CURSOR_DEFAULT, "Select", true);
	DrawToolBarButton(ImGuiGizmo::EMode::Translation, ICON_MDI_ARROW_ALL, "Translate", false);
	DrawToolBarButton(ImGuiGizmo::EMode::Rotatation, ICON_MDI_ROTATE_ORBIT, "Rotate", false);
	DrawToolBarButton(ImGuiGizmo::EMode::Scaling, ICON_MDI_RESIZE, "Scale", true);
}

void SceneView::DrawToolBarButton(ImGuiGizmo::EMode GizmoMode, const char* const Icon, const char* const Tooltip, bool DrawSeperator)
{
	static const Math::Color HightlightColor(0.28f, 0.56f, 0.9f, 1.0f);

	IMGUI_SCOPED_COLOR_CONDITION(ImGuiCol_Text, HightlightColor, m_GizmoMode == GizmoMode);
	
	ImGui::SameLine();
	if (ImGui::Button(Icon))
	{
		m_GizmoMode = GizmoMode;
	}
	ImGui::DrawToolTip(Tooltip);

	if (DrawSeperator)
	{
		ImGui::SameLine();
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
	}

	ImGui::SameLine();
}
