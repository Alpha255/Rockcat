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

	virtual EKeyboardKey MapKeyCode(uint64_t KeyCode, int64_t Mask) = 0;

	virtual void Render() {}
	virtual void RenderGUI() {}

	void DispatchKeyUpMessage(const KeyEvent& Event);
	void DispatchKeyDownMessage(const KeyEvent& Event);
	void DispatchKeyCharMessage(const char Character);

	void DispatchMouseButtonUpMessage(const EMouseButton Button);
	void DispatchMouseButtonDownMessage(const EMouseButton Button, const Math::Vector2& CursorPos);
	void DispatchMouseButtonDoubleClickMessage(const EMouseButton Button, const Math::Vector2& CursorPos);
	void DispatchMouseMoveMessage(const Math::Vector2& CursorPos);
	void DispatchMouseWheelMessage(float WheelDelta, const Math::Vector2& CursorPos);

	void DispatchWindowResizedMessage(uint32_t Width, uint32_t Height);
	void DispatchAppActiveChangedMessage();
	void DispatchAppInactiveChangedMessage();
	void DispatchAppDestroyMessage();

	std::shared_ptr<struct ApplicationSettings> m_Settings;

	std::unique_ptr<class Window> m_Window;
	std::unique_ptr<class RHIDevice> m_RenderDevice;

	KeyModifiers m_KeyModifiers;
	std::vector<class MessageHandler*> m_MessageHandlers;
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

#define REGISTER_APPLICATION(ApplicationClass, SettingsFile)

#define REGISTER_AND_RUN_APPLICATION(ApplicationClass, SettingsFile)