#pragma once

#include "Application/Window.h"

class WindowsWindow : public Window
{
public:
	WindowsWindow(uint32_t Width, uint32_t Height, const char* Title = nullptr);

	virtual void SetMode(EWindowMode Mode);

	virtual void OnResize(uint32_t Width, uint32_t Height);

	virtual void SetTitle(const char* Title);
};