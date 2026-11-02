#pragma once

#include "Application/Window.h"

#if PLATFORM_WINDOWS

#include <Windows.h>

#ifdef CreateWindow
	#undef CreateWindow
#endif

#ifdef RegisterClass
	#undef RegisterClass
#endif

#ifdef GetEnvironmentVariable
	#undef GetEnvironmentVariable
#endif

#ifdef SetEnvironmentVariable
	#undef SetEnvironmentVariable
#endif

class WindowsWindow : public Window
{
public:
	WindowsWindow(HINSTANCE HInstance, uint32_t Width, uint32_t Height, const char* Title = nullptr);

	virtual void SetMode(EWindowMode Mode);

	virtual void OnResize(uint32_t Width, uint32_t Height);

	virtual void SetTitle(const char* Title);

	static const std::wstring_view WindowClassName;
};

#endif