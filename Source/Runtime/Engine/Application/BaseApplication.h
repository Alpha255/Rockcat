#pragma once

#include "Core/Definitions.h"
#include "Core/InputState.h"
#include "Engine/Tickable.h"

class BaseApplication : public NoneCopyable, public ITickable, public IInputHandler
{
public:
	BaseApplication(const char* ConfigurationName);

	virtual ~BaseApplication() = default;

	virtual void Startup();
	virtual void RenderFrame() {}
	virtual void Shutdown() {}

	virtual bool IsRequestQuit() const;

	void Tick(float ElapsedSeconds) override;

	const class Window& GetWindow() const { return *m_Window; }
	const class ApplicationConfigurations& GetConfigurations() const;
	const struct GraphicsSettings& GetGraphicsSettings() const;
protected:
	virtual void MakeWindow();
	virtual void Initialize() {}
	virtual void RenderGUI(class Canvas&) {}
private:
	std::shared_ptr<class ApplicationConfigurations> m_Configs;
	std::shared_ptr<class Window> m_Window;
};