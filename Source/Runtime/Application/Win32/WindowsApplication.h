#pragma once

#include "Runtime/Application/BaseApplication.h"
#include <Windows.h>

#if PLATFORM_WIN32

class WindowsApplication : public BaseApplication
{
public:
	WindowsApplication(const char* SettingsFile);
protected:
	static LRESULT CALLBACK AppMessageProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

	LRESULT MessageProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

	virtual EKeyboardKey MapKeyCode(uint64_t KeyCode, int64_t Mask) override final;

	static WindowsApplication* s_WindowsApp;
};

using PlatformApplication = WindowsApplication;

#endif
