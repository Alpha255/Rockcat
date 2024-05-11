#pragma once

#include "Engine/Application/BaseApplication.h"

class ImGuiEditor final : public BaseApplication
{
public:
	using BaseApplication::BaseApplication;
protected:
	void OnInitialize() override final;
	void OnGUI(class Canvas&) override final {}
	void OnRenderFrame() override final;
	void OnMouseEvent(const MouseEvent& Mouse) override final;
	void OnKeyboardEvent(const KeyboardEvent& Keyboard) override final;
	void OnWindowResized(uint32_t Width, uint32_t Height) override final;
	void OnShutdown() override final;
private:
};