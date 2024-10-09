#pragma once

#include "Editor/ImGuiPanel.h"
#include "Editor/ImGuiGizmo.h"

class SceneView : public ImGuiPanel
{
public:
	SceneView();
protected:
	void OnDraw() override final;
private:
	void DrawToolBar();
	void DrawToolBarButton(ImGuiGizmo::EMode GizmoMode, const char* const Icon, const char* const Tooltip, bool DrawSeperator);

	ImGuiGizmo::EMode m_GizmoMode = ImGuiGizmo::EMode::None;
};