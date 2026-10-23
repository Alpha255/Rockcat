#pragma once

#include "Core/Definitions.h"

#if PLATFORM_WINDOWS

#include "Application/Windows/WindowsPlatformMisc.hpp"

class WindowsApplication : public Application
{
public:
	WindowsApplication();

	void PumpMessages() override final;

	virtual std::shared_ptr<class Window> CreateWindow(uint32_t Width, uint32_t Height, const char* Title) override final;
protected:
	static LRESULT CALLBACK ApplicationMessageProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

	LRESULT MessageProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

	virtual EKeyboardKey DecodeKeyboardKey(uint64_t KeyCode, int64_t Mask, KeyModifiers& Modifiers) override final;

	void RegisterClass();
};

#endif
