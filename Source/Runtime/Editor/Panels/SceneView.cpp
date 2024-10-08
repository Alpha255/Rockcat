#include "Editor/Panels/SceneView.h"
#include <Submodules/IconFontCppHeaders/IconsMaterialDesignIcons.h>

SceneView::SceneView()
	: ImGuiPanel(
		"Scene", 
		ICON_MDI_CONTROLLER_OFF,
		nullptr,
		ImGuiWindowFlags_NoCollapse | 
		ImGuiWindowFlags_NoScrollbar | 
		ImGuiWindowFlags_NoScrollWithMouse)
{
}

void SceneView::OnDraw()
{
}
