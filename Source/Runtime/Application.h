#pragma once

#include "Runtime/Scene.h"
#include "Runtime/Rockcat.hpp"
#include "Colorful/IRenderer/IRenderer.h"

namespace RHI
{
	class ImGUIPass;
}

class Application : public NoneCopyable, public ITickable, public IInputHandler
{
public:
	Application(const char8_t* SettingPath = "Default.json");

	virtual ~Application();

	virtual void OnStartup() {}

	virtual void Render();

	virtual void RenderGUI() {}

	void OnMouseEvent(const MouseEvent& Mouse) override;

	void OnKeyboardEvent(const KeyboardEvent& Keyboard) override;

	void OnWindowResized(uint32_t Width, uint32_t Height) override;

	void Tick(float32_t ElapsedSeconds) override;

	const RHI::RenderSettings* RenderSettings() const
	{
		return m_RenderSettings.get();
	}
protected:
	void RenderScene(RHI::IFrameBuffer* SwapchainFrameBuffer);

	std::shared_ptr<RHI::RenderSettings> m_RenderSettings;
	std::shared_ptr<Scene> m_Scene;
	std::unique_ptr<RHI::IFrameGraph> m_FrameGraph;
	RHI::ImGUIPass* m_GUIPass = nullptr;
	Camera* m_Camera = nullptr;
	static RHI::IRenderer* GRenderer;
};