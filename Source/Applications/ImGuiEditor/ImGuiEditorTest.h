#pragma once

#include "Application/BaseApplication.h"
#include "EventRouter.h"
#include <imgui/imgui.h>

class ImGuiEditorTest final : public BaseApplication
{
public:
	using BaseApplication::BaseApplication;

	void Finalize() override final;
	void Tick(float ElapsedSeconds) override final;

	void OnMouseMoveEvent(const MouseMoveEvent& Event) override final;
	void OnMouseButtonEvent(const MouseButtonEvent& Event) override final;
	void OnMouseWheelEvent(const MouseWheelEvent& Event) override final;
	void OnKeyEvent(const KeyEvent& Event) override final;
	void OnWindowResized(uint32_t Width, uint32_t Height) override final;

	void Initialize() override final;
	void Render(RHITexture* RenderSurface) override final;
protected:
	void RenderGUI() override final;
private:
	std::shared_ptr<class ImGuiEditor> m_Editor;
};