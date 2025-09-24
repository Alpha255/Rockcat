#pragma once

#include "Runtime/Application/BaseApplication.h"
#include <Windows.h>

#if PLATFORM_WIN32

class WindowsApplication : public BaseApplication
{
public:
	using BaseApplication::BaseApplication;
protected:
	static LRESULT CALLBACK MessageProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

	virtual EKeyboardKey MapKeyCode(int32_t KeyCode, int64_t Mask) override final;
};

using PlatformApplication = WindowsApplication;

#endif
