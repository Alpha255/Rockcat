#pragma once

#include "Core/InputState.h"
#include "Engine/Tickable.h"

class BaseApplication : public NoneCopyable, public ITickable, public IInputHandler
{
public:
	BaseApplication(const char* ConfigurationPath);

	virtual ~BaseApplication() = default;

	virtual void Startup();
	virtual void RenderFrame() {}
	virtual void Shutdown() {}
	virtual bool IsRequestQuit() const;

	void Tick(float ElapsedSeconds) override;

	const class Window& GetWindow();
	class RHIBackend& GetRenderBackend();
	const struct RenderSettings& GetRenderSettings() const;
protected:
	friend class RenderService;

	void MakeWindow();
	virtual void Initialize() {}
	virtual void RenderGUI(class Canvas&) {}

	void SetRenderBackend(const std::shared_ptr<class RHIBackend>& Backend);
private:
	std::shared_ptr<struct ApplicationConfiguration> m_Configs;
	std::shared_ptr<class Window> m_Window;
	std::shared_ptr<class RHIBackend> m_RenderBackend;
	std::shared_ptr<class RHIViewport> m_Viewport;
};