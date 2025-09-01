#pragma once

#include "Core/Tickable.h"
#include "EventRouter.h"

class BaseApplication : public NoneCopyable, public ITickable, public EventHandler
{
public:
	BaseApplication(const char* SettingsFile = nullptr);
	virtual ~BaseApplication();

	virtual void Initialize() {}
	virtual void Render() {}
	virtual void RenderGUI() {}
	virtual void Finalize() {}
	
	virtual void PumpMessages();

	virtual void Run();

	void OnWindowStatusChanged(EWindowStatus Status) override;
	
	void Tick(float ElapsedSeconds) override;

	bool IsActivate() const;
	bool IsRequestQuit() const;
protected:
	bool InitializeRHI();
	void FinalizeRHI();

	std::shared_ptr<struct ApplicationSettings> m_Settings;

	std::unique_ptr<class Window> m_Window;
	std::unique_ptr<class RHIDevice> m_RenderDevice;

	std::unique_ptr<class CpuTimer> m_Timer;
};

struct RunApplication
{
	using CreateAppFunc = std::function<BaseApplication*(void)>;

	CreateAppFunc CreateApplication;

	RunApplication(CreateAppFunc&& Func)
		: CreateApplication(std::move(Func))
	{
	}
};

#define RUN_APPLICATION(Application, SettingsFile) RunApplication g_RunApplication([](){ return new Application(SettingsFile); });