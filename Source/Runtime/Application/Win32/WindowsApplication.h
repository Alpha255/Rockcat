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
};

using PlatformApplication = WindowsApplication;

#endif
