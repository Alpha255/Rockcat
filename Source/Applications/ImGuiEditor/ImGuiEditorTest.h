#pragma once

#include "Engine/Application/BaseApplication.h"
#include <Submodules/imgui/imgui.h>

class ImGuiEditorTest final : public BaseApplication
{
public:
	using BaseApplication::BaseApplication;
protected:
	void Initialize() override final;
	void RenderGUI(class Canvas&) override final;
	void RenderFrame() override final;
	void OnMouseEvent(const MouseEvent& Mouse) override final;
	void OnKeyboardEvent(const KeyboardEvent& Keyboard) override final;
	void OnWindowResized(uint32_t Width, uint32_t Height) override final;
	void Shutdown() override final;
private:
	std::shared_ptr<class ImGuiEditor> m_Editor;
};