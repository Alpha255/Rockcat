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
	{
		break;
	}
	case WM_KEYUP:
	case WM_SYSKEYUP:
	{
		break;
	}
	case WM_GETMINMAXINFO:
		break;
	case WM_CHAR:
		break;
	case WM_INPUT:
		break;
	}

    return ::DefWindowProcA(hWnd, Msg, wParam, lParam);
}

EKeyboardKey WindowsApplication::MapKeyCode(int32_t KeyCode, int64_t Mask)
{
	EKeyboardKey ActualKeyCode = EKeyboardKey::None;

	switch (KeyCode)
	{
	case VK_ESCAPE:
		ActualKeyCode = EKeyboardKey::Esc;
		break;
	case VK_TAB:
		ActualKeyCode = EKeyboardKey::Tab;
		break;
	case VK_RETURN:
		ActualKeyCode = EKeyboardKey::Enter;
		break;
	case VK_SHIFT:
		if ((Mask & 0x1000000) == 0)
		{
			ActualKeyCode = EKeyboardKey::LeftShift;
			m_KeyModifiers.LeftShift = !m_KeyModifiers.LeftShift;
		}
		else
		{
			ActualKeyCode = EKeyboardKey::RightShift;
			m_KeyModifiers.RightShift = !m_KeyModifiers.RightShift;
		}
		break;
	case VK_CONTROL:
		if ((Mask & 0x1000000) == 0)
		{
			ActualKeyCode = EKeyboardKey::LeftCtrl;
			m_KeyModifiers.LeftCtrl = !m_KeyModifiers.LeftCtrl;
		}
		else
		{
			ActualKeyCode = EKeyboardKey::RightCtrl;
			m_KeyModifiers.RightCtrl = !m_KeyModifiers.RightCtrl;
		}
		break;
	case VK_MENU:
		if ((Mask & 0x1000000) == 0)
		{
			ActualKeyCode = EKeyboardKey::LeftAlt;
			m_KeyModifiers.LeftAlt = !m_KeyModifiers.LeftAlt;
		}
		else
		{
			ActualKeyCode = EKeyboardKey::RightAlt;
			m_KeyModifiers.RightAlt = !m_KeyModifiers.RightAlt;
		}
		break;
	case VK_CAPITAL:
		ActualKeyCode = EKeyboardKey::CapsLock;
		m_KeyModifiers.CapsLock = (::GetKeyState(VK_CAPITAL) & 0x0001) != 0;
		break;
	case VK_INSERT:
		ActualKeyCode = EKeyboardKey::Insert;
		break;
	case VK_DELETE:
		ActualKeyCode = EKeyboardKey::Delete;
		break;
	case VK_HOME:
		ActualKeyCode = EKeyboardKey::Home;
		break;
	case VK_END:
		ActualKeyCode = EKeyboardKey::End;
		break;
	case VK_PRIOR:
		ActualKeyCode = EKeyboardKey::PageUp;
		break;
	case VK_NEXT:
		ActualKeyCode = EKeyboardKey::PageDown;
		break;
	case VK_LEFT:
		ActualKeyCode = EKeyboardKey::Left;
		break;
	case VK_UP:
		ActualKeyCode = EKeyboardKey::Up;
		break;
	case VK_RIGHT:
		ActualKeyCode = EKeyboardKey::Right;
		break;
	case VK_DOWN:
		ActualKeyCode = EKeyboardKey::Down;
		break;
	case VK_F1:
		ActualKeyCode = EKeyboardKey::F1;
		break;
	case VK_F2:
		ActualKeyCode = EKeyboardKey::F2;
		break;
	case VK_F3:
		ActualKeyCode = EKeyboardKey::F3;
		break;
	case VK_F4:
		ActualKeyCode = EKeyboardKey::F4;
		break;
	case VK_F5:
		ActualKeyCode = EKeyboardKey::F5;
		break;
	case VK_F6:
		ActualKeyCode = EKeyboardKey::F6;
		break;
	case VK_F7:
		ActualKeyCode = EKeyboardKey::F7;
		break;
	case VK_F8:
		ActualKeyCode = EKeyboardKey::F8;
		break;
	case VK_F9:
		ActualKeyCode = EKeyboardKey::F9;
		break;
	case VK_F10:
		ActualKeyCode = EKeyboardKey::F10;
		break;
	case VK_F11:
		ActualKeyCode = EKeyboardKey::F11;
		break;
	case VK_F12:
		ActualKeyCode = EKeyboardKey::F12;
		break;
	case VK_SPACE:
		ActualKeyCode = EKeyboardKey::Space;
		break;
	case VK_LWIN:
		ActualKeyCode = EKeyboardKey::LeftMenu;
		break;
	case VK_RWIN:
		ActualKeyCode = EKeyboardKey::RightMenu;
		break;
	case VK_BACK:
		ActualKeyCode = EKeyboardKey::Backspace;
		break;
	default:
		if ((KeyCode >= '0' && KeyCode <= '9') || (KeyCode >= 'A' && KeyCode <= 'Z'))
		{
			ActualKeyCode = static_cast<EKeyboardKey>(KeyCode);
		}
		else
		{
			ActualKeyCode = EKeyboardKey::OEM;
		}
		break;
	}

	return ActualKeyCode;
}
