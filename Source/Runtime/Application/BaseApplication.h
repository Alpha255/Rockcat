#pragma once

#include "Core/Tickable.h"
#include "EventRouter.h"

class BaseApplication : public NoneCopyable
{
public:
	BaseApplication(const char* SettingsFile = nullptr);
	virtual ~BaseApplication();

	virtual void Initialize() {}
	virtual void Finalize() {}

	virtual void RenderFrame();

	virtual void Tick();

	virtual void Run();
	
	virtual void PumpMessages();

	virtual bool IsActive() const;
	virtual bool IsRequestQuit() const;
protected:
	bool InitializeRHI();
	void FinalizeRHI();

	virtual EKeyboardKey MapKeyCode(int32_t KeyCode, int64_t Mask) = 0;

	virtual void Render() {}
	virtual void RenderGUI() {}

	std::shared_ptr<struct ApplicationSettings> m_Settings;

	std::unique_ptr<class Window> m_Window;
	std::unique_ptr<class RHIDevice> m_RenderDevice;

	KeyModifiers m_KeyModifiers;
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