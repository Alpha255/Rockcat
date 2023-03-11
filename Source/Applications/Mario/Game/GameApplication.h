#pragma once

#include "IApplication.h"
#include "Timer.h"

class GameApplication : public IApplication
{
public:
	enum eOriginalResolution
	{
		eWidth = 640,
		eHeight = 480
	};

	GameApplication();
	~GameApplication() = default;

	virtual	LRESULT MsgProc(HWND hWnd, uint32_t msg, WPARAM wParam, LPARAM lParam);
	virtual void RenderToWindow();
	virtual void ResizeWindow(uint32_t width, uint32_t height);
	virtual void InitRenderer() override;
protected:
private:
	bool m_bInited = false;
};


