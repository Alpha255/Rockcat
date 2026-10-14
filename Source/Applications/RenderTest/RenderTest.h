#pragma once

#include "Application/PlatformApplication.h"
#include "Misc/MessageHandler.h"

#include "RHI/Vulkan/VulkanImGuiRenderer.h"

class RenderTest final : public PlatformApplication, public MessageHandler
{
public:
	using PlatformApplication::PlatformApplication;

	void Initialize() override final;
	void Render() override final;
	void Finalize() override final;

	/// MessageHandler: relays the platform input to ImGui. Everything returns whether ImGui wants
	/// to capture the event, so the application can decide to act on it as well.
	bool OnKeyDown(const KeyEvent& Event, const KeyModifiers& Modifiers) override final;
	bool OnKeyUp(const KeyEvent& Event, const KeyModifiers& Modifiers) override final;
	bool OnKeyChar(const char Character) override final;

	bool OnMouseButtonDown(const EMouseButton Button, const Math::Vector2& CursorPos) override final;
	bool OnMouseButtonUp(const EMouseButton Button) override final;
	bool OnMouseMove(const Math::Vector2& CursorPos) override final;
	bool OnMouseWheel(const float WheelDelta, const Math::Vector2& CursorPos) override final;

protected:
private:
	/// Minimal renderable skeleton:
	///   acquire back buffer -> clear -> ui draw commands -> present
	/// Deliberately does not go through BaseApplication::RenderScene().
	void DrawFrame();

	void InitializeImGui();

	/// Keeps the frame buffer at the client area of the window (both are device pixels for a per
	/// monitor DPI aware process) and the UI density in sync with the monitor: fonts and style are
	/// rebuilt whenever the scale moves, see ImGuiConfiguration::SetDisplayScale().
	void UpdateDisplay();

	std::shared_ptr<class Scene> m_Scene;
	std::shared_ptr<class SceneView> m_SceneView;

	/// UI density (device pixels per logical pixel) the ImGui atlas and style were last built for;
	/// UpdateDisplay() acts on the difference.
	float m_DisplayScale = 0.0f;

	/// Owns the ImGui context (its constructor creates it) and loads the fonts and the theme.
	std::unique_ptr<class ImGuiEditor> m_Editor;

	VulkanImGuiRenderer m_ImGuiRenderer;
};