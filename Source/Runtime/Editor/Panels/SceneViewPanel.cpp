#include "Editor/Panels/SceneViewPanel.h"
#include "Editor/Icons/Icons.h"

SceneViewPanel::SceneViewPanel()
	: ImGuiPanel(
		"Scene", 
		ICON_SCENE_VIEW,
		nullptr,
		ImGuiWindowFlags_NoCollapse | 
		ImGuiWindowFlags_NoScrollbar | 
		ImGuiWindowFlags_NoScrollWithMouse)
{
}

void SceneViewPanel::OnDraw()
{
	DrawToolBar();
}

void SceneViewPanel::DrawToolBar()
{
	ImGui::Indent();
	IMGUI_SCOPED_COLOR(ImGuiCol_Button, Math::Color::Black);

	DrawToolBarButton(ICON_OBJ_MODE_SELECT, "Select", m_GizmoMode == ImGuiGizmo::EMode::Selection, true, [this]()
		{
			m_GizmoMode = ImGuiGizmo::EMode::Selection;
		}
	);
	DrawToolBarButton(ICON_OBJ_MODE_TRANS, "Translate", m_GizmoMode == ImGuiGizmo::EMode::Translation, false, [this]()
		{
			m_GizmoMode = ImGuiGizmo::EMode::Translation;
		}
	);
	DrawToolBarButton(ICON_OBJ_MODE_ROTATE, "Rotate", m_GizmoMode == ImGuiGizmo::EMode::Rotatation, false, [this]()
		{
			m_GizmoMode = ImGuiGizmo::EMode::Rotatation;
		}
	);
	DrawToolBarButton(ICON_OBJ_MODE_SCALE, "Scale", m_GizmoMode == ImGuiGizmo::EMode::Scaling, true, [this]()
		{
			m_GizmoMode = ImGuiGizmo::EMode::Scaling;
		}
	);

	DrawToolBarButton(ICON_VIEW_MODE_3D " 3D", "3D", true, false, [this]()
		{
		}
	);
	DrawToolBarButton(ICON_VIEW_MODE_2D " 2D", "2D", false, false, [this]()
		{
		}
	);
	ImGui::Unindent();
}

void SceneViewPanel::DrawToolBarButton(const char* const Label, const char* const Tooltip, bool Highlight, bool DrawSeperator, std::function<void()>&& BtnClickCallback)
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
