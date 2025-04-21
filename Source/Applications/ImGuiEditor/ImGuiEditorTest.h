#pragma once

#include "Engine/Application/BaseApplication.h"
#include "Core/MessageRouter.h"
#include <Submodules/imgui/imgui.h>

class ImGuiEditorTest final : public BaseApplication, public MessageHandler
{
public:
	using BaseApplication::BaseApplication;

	void Finalize() override final;
	void Tick(float ElapsedSeconds) override final;

	void OnMouseEvent(const MouseEvent& Mouse) override final;
	void OnKeyboardEvent(const KeyboardEvent& Keyboard) override final;
	void OnWindowResized(uint32_t Width, uint32_t Height) override final;

	void RenderFrame(class RHITexture* BackBuffer) override final;
protected:
	void InitializeImpl() override final;
	void RenderGUI(class Canvas&) override final;
private:
	std::shared_ptr<class ImGuiEditor> m_Editor;
};