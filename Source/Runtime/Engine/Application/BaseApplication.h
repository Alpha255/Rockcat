#pragma once

#include "Core/Definitions.h"
#include "Core/InputState.h"
#include "Engine/Tickable.h"

class BaseApplication : public NoneCopyable, public ITickable, public IInputHandler
{
public:
	BaseApplication(const char* ConfigurationName);

	virtual ~BaseApplication() = default;

	virtual void OnStartup();
	virtual void OnRenderFrame() {}
	virtual void OnShutdown() {}

	void OnMouseEvent(const MouseEvent& /*Mouse*/) override {}
	void OnKeyboardEvent(const KeyboardEvent& /*Keyboard*/) override {}
	void OnWindowResized(uint32_t /*Width*/, uint32_t /*Height*/) override {}

	void Tick(float ElapsedSeconds) override;

	class Window* GetAssociatedWindow() const { return m_Window.get(); }
	const class ApplicationConfigurations& GetConfigurations() const;
	const struct GraphicsSettings& GetGraphicsSettings() const;

	bool IsRequestingQuit() const;
protected:
	virtual void OnWindowCreation();
	virtual void OnInitialize() {}
	virtual void OnRenderGUI(class Canvas&) {}
private:
	std::shared_ptr<class ApplicationConfigurations> m_Configs;
	std::shared_ptr<class Window> m_Window;
};