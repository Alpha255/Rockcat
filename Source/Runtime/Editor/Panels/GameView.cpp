#include "Editor/Panels/GameView.h"
#include <Submodules/IconFontCppHeaders/IconsMaterialDesignIcons.h>

GameView::GameView()
	: ImGuiPanel(
		"Game", 
		ICON_MDI_CONTROLLER,
		nullptr,
		ImGuiWindowFlags_NoCollapse | 
		ImGuiWindowFlags_NoScrollbar | 
		ImGuiWindowFlags_NoScrollWithMouse)
{
}

void GameView::OnDraw()
{
}
