#pragma once

#include "Editor/ImGuiPanel.h"
#include "Editor/ImGuiGizmo.h"

class SceneViewPanel : public ImGuiPanel
{
public:
	SceneViewPanel();
protected:
	void OnDraw() override final;
private:
	void DrawToolBar();
	void DrawToolBarButton(const char* const Label, const char* const Tooltip, bool Highlight, bool DrawSeperator, std::function<void()>&& BtnClickCallback);

	ImGuiGizmo::EMode m_GizmoMode = ImGuiGizmo::EMode::None;
};