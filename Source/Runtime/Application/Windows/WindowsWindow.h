#pragma once

#include "Application/Window.h"

#if PLATFORM_WINDOWS

#include "Application/Windows/WindowsPlatformMisc.hpp"

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