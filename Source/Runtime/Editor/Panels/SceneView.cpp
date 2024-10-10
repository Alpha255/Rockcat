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

	DrawToolBarButton(ICON_MDI_CURSOR_DEFAULT, "Select", m_GizmoMode == ImGuiGizmo::EMode::Selection, true, [this]() 
		{
			m_GizmoMode = ImGuiGizmo::EMode::Selection;
		}
	);
	DrawToolBarButton(ICON_MDI_ARROW_ALL, "Translate", m_GizmoMode == ImGuiGizmo::EMode::Translation, false, [this]()
		{
			m_GizmoMode = ImGuiGizmo::EMode::Translation;
		}
	);
	DrawToolBarButton(ICON_MDI_ROTATE_ORBIT, "Rotate", m_GizmoMode == ImGuiGizmo::EMode::Rotatation, false, [this]()
		{
			m_GizmoMode = ImGuiGizmo::EMode::Rotatation;
		}
	);
	DrawToolBarButton(ICON_MDI_RESIZE, "Scale", m_GizmoMode == ImGuiGizmo::EMode::Scaling, true, [this]()
		{
			m_GizmoMode = ImGuiGizmo::EMode::Scaling;
		}
	);

	DrawToolBarButton(ICON_MDI_AXIS_ARROW " 3D", "3D", true, false, [this]() 
		{
		}
	);
	DrawToolBarButton(ICON_MDI_ANGLE_RIGHT " 2D", "2D", false, false, [this]()
		{
		}
	);
	ImGui::Unindent();
}

void SceneView::DrawToolBarButton(const char* const Label, const char* const Tooltip, bool Highlight, bool DrawSeperator, std::function<void()>&& BtnClickCallback)
{
	static const Math::Color HightlightColor(0.28f, 0.56f, 0.9f, 1.0f);

	{
		IMGUI_SCOPED_COLOR_CONDITION(ImGuiCol_Text, HightlightColor, Highlight);

		ImGui::SameLine();
		if (ImGui::Button(Label))
		{
			BtnClickCallback();
		}
	}

	ImGui::DrawToolTip(Tooltip);

	if (DrawSeperator)
	{
		ImGui::SameLine();
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
	}

	ImGui::SameLine();
}
