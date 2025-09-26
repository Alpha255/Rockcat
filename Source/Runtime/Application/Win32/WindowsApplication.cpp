#include "Application/Win32/WindowsApplication.h"
#include "Application/Win32/Resource.h"
#include "Application/ApplicationSettings.h"
#include "Services/SpdLogService.h"
#include "System/System.h"
#include <windowsx.h>

#if PLATFORM_WIN32

WindowsApplication* WindowsApplication::s_Application = nullptr;

WindowsApplication::WindowsApplication(const char* SettingsFile)
	: BaseApplication(SettingsFile)
{
	s_Application = this;

	HINSTANCE HInstance = reinterpret_cast<HINSTANCE>(System::GetApplicationInstance());
	VERIFY_WITH_SYSTEM_MESSAGE(HInstance);

	HICON HIcon = ::LoadIconW(HInstance, MAKEINTRESOURCEW(ICON_NVIDIA));
	VERIFY_WITH_SYSTEM_MESSAGE(HIcon);

	WNDCLASSEXW WndClassEx
	{
		sizeof(WNDCLASSEXW),
		CS_HREDRAW | CS_VREDRAW,
		AppMessageProc,
		0,
		sizeof(void*),
		HInstance,
		HIcon,
		::LoadCursorW(0, IDC_ARROW),
		static_cast<HBRUSH>(::GetStockObject(DKGRAY_BRUSH)),
		nullptr,
		WINDOW_CLASS_NAME,
		HIcon
	};
	VERIFY_WITH_SYSTEM_MESSAGE(::RegisterClassExW(&WndClassEx) != 0);
}

void WindowsApplication::PumpMessages()
{
	MSG Message;
	if (::PeekMessageW(&Message, nullptr, 0u, 0u, PM_REMOVE))
	{
		::TranslateMessage(&Message);
		::DispatchMessageW(&Message);
	}
}

LRESULT WindowsApplication::AppMessageProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	return s_Application->MessageProc(hWnd, Msg, wParam, lParam);
}

LRESULT WindowsApplication::MessageProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	auto GetCursorPos = [this, &hWnd](LPARAM lParam) -> Math::Vector2 {
		POINT CursorPos
		{
			GET_X_LPARAM(lParam),
			GET_Y_LPARAM(lParam)
		};
		::ClientToScreen(hWnd, &CursorPos);

		return Math::Vector2(CursorPos.x, CursorPos.y);
	};

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
		DispatchAppQuitMessage();
		SetStatus(EApplicationStatus::Quit);
		break;
	case WM_NCLBUTTONDBLCLK:
		break;
	case WM_LBUTTONDOWN:
		DispatchMouseButtonDownMessage(EMouseButton::Left, GetCursorPos(lParam));
		break;
	case WM_LBUTTONUP:
		DispatchMouseButtonUpMessage(EMouseButton::Left);
		break;
	case WM_LBUTTONDBLCLK:
		DispatchMouseButtonDoubleClickMessage(EMouseButton::Left, GetCursorPos(lParam));
		break;
	case WM_RBUTTONDOWN:
		DispatchMouseButtonDownMessage(EMouseButton::Right, GetCursorPos(lParam));
		break;
	case WM_RBUTTONUP:
		DispatchMouseButtonUpMessage(EMouseButton::Right);
		break;
	case WM_RBUTTONDBLCLK:
		DispatchMouseButtonDoubleClickMessage(EMouseButton::Right, GetCursorPos(lParam));
		break;
	case WM_MBUTTONDOWN:
		DispatchMouseButtonDownMessage(EMouseButton::Middle, GetCursorPos(lParam));
		break;
	case WM_MBUTTONUP:
		DispatchMouseButtonUpMessage(EMouseButton::Middle);
		break;
	case WM_MBUTTONDBLCLK:
		DispatchMouseButtonDoubleClickMessage(EMouseButton::Middle, GetCursorPos(lParam));
		break;
	case WM_MOUSEWHEEL:
	{
		static const float s_SpinFactor = 1.0f / WHEEL_DELTA;
		const short WheelDelta = GET_WHEEL_DELTA_WPARAM(wParam);
		DispatchMouseWheelMessage(WheelDelta * s_SpinFactor, GetCursorPos(lParam));
		break;
	}
	case WM_MOUSEMOVE:
	case WM_NCMOUSEMOVE:
		DispatchMouseMoveMessage(GetCursorPos(lParam));
		break;
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
	{
		auto ActualKeyCode = MapKeyCode(wParam, lParam);
		DispatchKeyDownMessage(KeyEvent{ ActualKeyCode, wParam });
		LOG_DEBUG("Key \"{}\" down", magic_enum::enum_name(ActualKeyCode).data());
		break;
	}
	case WM_KEYUP:
	case WM_SYSKEYUP:
	{
		auto ActualKeyCode = MapKeyCode(wParam, lParam);
		DispatchKeyUpMessage(KeyEvent{ ActualKeyCode, wParam });
		LOG_DEBUG("Key \"{}\" up", magic_enum::enum_name(ActualKeyCode).data());
		break;
	}
	case WM_GETMINMAXINFO:
	{
		::LPMINMAXINFO MinMaxInfo = reinterpret_cast<::LPMINMAXINFO>(lParam);
		assert(MinMaxInfo);
		MinMaxInfo->ptMinTrackSize =
		{
			MINIMAL_WINDOW_SIZE,
			MINIMAL_WINDOW_SIZE
		};
		break;
	}
	case WM_CHAR:
	{
		const wchar_t wCharacter = static_cast<wchar_t>(wParam);
		const auto Character = StringUtils::ToMultiByte(&wCharacter);
		DispatchKeyCharMessage(Character[0]);
		break;
	}
	case WM_INPUT:
		break;
	default:
		return ::DefWindowProcW(hWnd, Msg, wParam, lParam);
	}

	return 0;
}

EKeyboardKey WindowsApplication::MapKeyCode(uint64_t KeyCode, int64_t Mask)
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

#endif
