#include "Editor/Panels/SceneView.h"
#include "Assets/Fonts/Icons/Blender/IconsBlenderIcons.h"

SceneView::SceneView()
	: ImGuiPanel(
		"Scene", 
		ICON_BLENDER_SCENE,
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

	DrawToolBarButton(ImGuiGizmo::EMode::Selection, ICON_BLENDER_RESTRICT_SELECT_OFF, "Select", true);
	DrawToolBarButton(ImGuiGizmo::EMode::Translation, ICON_BLENDER_EMPTY_ARROWS, "Translate", false);
	DrawToolBarButton(ImGuiGizmo::EMode::Rotatation, ICON_BLENDER_ORIENTATION_GIMBAL, "Rotate", false);
	DrawToolBarButton(ImGuiGizmo::EMode::Scaling, ICON_BLENDER_OBJECT_ORIGIN, "Scale", true);
}

void SceneView::DrawToolBarButton(ImGuiGizmo::EMode GizmoMode, const char* const Icon, const char* const Tooltip, bool DrawSeperator)
{
	static const Math::Color HightlightColor(0.28f, 0.56f, 0.9f, 1.0f);

	IMGUI_SCOPED_COLOR_CONDITION(ImGuiCol_Text, HightlightColor, m_GizmoMode == GizmoMode);
	
	ImGui::SameLine();
	if (ImGui::Button(Icon))
	{
	}
	ImGui::DrawToolTip(Tooltip);

	if (DrawSeperator)
	{
		ImGui::SameLine();
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
	}

	ImGui::SameLine();
}
