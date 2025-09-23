#include "Runtime/Application/Win32/WindowsApplication.h"

LRESULT WindowsApplication::MessageProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	switch (Msg)
	{
	case WM_ACTIVATE:
		break;
	case WM_SIZE:
		break;
	case WM_ENTERSIZEMOVE:
		break;
	case WM_EXITSIZEMOVE:
		break;
	case WM_QUIT:
	case WM_DESTROY:
		::PostQuitMessage(0);
		break;
	case WM_NCLBUTTONDBLCLK:
		break;
	case WM_LBUTTONDOWN:
		break;
	case WM_LBUTTONUP:
		break;
	case WM_LBUTTONDBLCLK:
		break;
	case WM_RBUTTONDOWN:
		break;
	case WM_RBUTTONUP:
		break;
	case WM_RBUTTONDBLCLK:
		break;
	case WM_MBUTTONDOWN:
		break;
	case WM_MBUTTONUP:
		break;
	case WM_MBUTTONDBLCLK:
		break;
	case WM_MOUSEWHEEL:
		break;
	case WM_MOUSEMOVE:
		break;
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		switch (wParam)
		{
		case VK_MENU:
			break;
		case VK_CONTROL:
			break;
		case VK_SHIFT:
			break;
		case VK_CAPITAL:
			break;
		default:
			break;
		}
		break;
	case WM_KEYUP:
	case WM_SYSKEYUP:
		break;
	case WM_GETMINMAXINFO:
		break;
	case WM_CHAR:
		break;
	}

    return ::DefWindowProcA(hWnd, Msg, wParam, lParam);
}
