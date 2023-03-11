#include "GameApplication.h"
#include "resource.h"
#include "Engine.h"

GameApplication::GameApplication()
{
	m_IconID = IconMario;
}

LRESULT GameApplication::MsgProc(HWND hWnd, uint32_t msg, WPARAM wParam, LPARAM lParam)
{
	assert(m_Timer);

	HandleWindowMessage(msg, wParam, lParam);

	Engine::Instance().HandleInput(msg, wParam, lParam);

	return ::DefWindowProc(hWnd, msg, wParam, lParam);
}

void GameApplication::RenderToWindow()
{
	if (!m_bInited)
	{
		Engine::Instance().Init(m_hWnd, m_Width, m_Height);
		m_bInited = true;
	}

	Engine::Instance().Update(m_Timer->GetDeltaTime(), m_Timer->GetTotalTime());

	Engine::Instance().RenderScene();
}

void GameApplication::ResizeWindow(uint32_t width, uint32_t height)
{
	Engine::Instance().Resize(width, height);
}

void GameApplication::InitRenderer()
{
}
