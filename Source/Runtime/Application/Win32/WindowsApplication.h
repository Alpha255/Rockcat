#pragma once

#include "Runtime/Application/BaseApplication.h"

#if PLATFORM_WIN32

#include <Windows.h>

class WindowsApplication : public BaseApplication
{
public:
	WindowsApplication(const char* SettingsFile);

	void PumpMessages() override final;
protected:
	static LRESULT CALLBACK AppMessageProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

	LRESULT MessageProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

	virtual EKeyboardKey MapKeyCode(uint64_t KeyCode, int64_t Mask) override final;

	static WindowsApplication* s_Application;
};

using PlatformApplication = WindowsApplication;

#endif
