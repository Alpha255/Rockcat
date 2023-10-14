#pragma once

#include "Runtime/Core/Definitions.h"
#include "Runtime/Core/InputState.h"
#include "Runtime/Engine/Tickable.h"

class BaseApplication : public NoneCopyable, public ITickable, public IInputHandler
{
public:
	BaseApplication(const char8_t* ConfigurationName);

	virtual ~BaseApplication() = default;

	virtual void OnStartup();

	virtual void OnRenderFrame();

	virtual void OnShutdown() {}

	void OnMouseEvent(const MouseEvent& /*Mouse*/) override {}

	void OnKeyboardEvent(const KeyboardEvent& /*Keyboard*/) override {}

	void OnWindowResized(uint32_t /*Width*/, uint32_t /*Height*/) override {}

	void Tick(float32_t ElapsedSeconds) override;

	class Window* GetAssociatedWindow() const { return m_Window.get(); }

	const class ApplicationConfigurations& GetConfigurations() const;

	bool8_t IsRequestingQuit() const;
protected:
	virtual void OnWindowCreation();
	virtual void OnInitialize() {}
	virtual void OnGUI(class Canvas&) {}
private:
	std::shared_ptr<class ApplicationConfigurations> m_Configs;
	std::shared_ptr<class Window> m_Window;
};