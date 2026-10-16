#include "Application/Win32/WindowsApplication.h"
#include "Application/Win32/Resource.h"
#include "Application/ApplicationSettings.h"
#include "Core/SpdLogging.h"
#include "Misc/PlatformMisc.h"
#include "Application/Window.h"
#include <windowsx.h>

#if PLATFORM_WIN32

extern BaseApplication* GApplication;

WindowsApplication::WindowsApplication(const char* SettingsFile)
	: BaseApplication(SettingsFile)
{
	/// Has to be the very first DPI dependent call in the process, well before the first HWND of
	/// the WNDCLASSEX below exists. See InitializeDpiAwareness().
	InitializeDpiAwareness();

	HINSTANCE HInstance = reinterpret_cast<HINSTANCE>(PlatformMisc::GetApplicationHandle());
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

	/// Drained instead of a single message per frame: a key press alone already arrives as three of
	/// them (down, char, up), and dropping to one per frame makes input lag behind the presentation,
	/// which stalls waiting for the queue on top of that.
	while (::PeekMessageW(&Message, nullptr, 0u, 0u, PM_REMOVE))
	{
		::TranslateMessage(&Message);
		::DispatchMessageW(&Message);
	}
}

LRESULT WindowsApplication::AppMessageProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	if (auto WinApp = Cast<WindowsApplication>(GApplication))
	{
		return WinApp->MessageProc(hWnd, Msg, wParam, lParam);
	}

	return E_FAIL;
}

LRESULT WindowsApplication::MessageProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	/// Everything downstream - ImGui above all - works in client coordinates: with multi viewports
	/// turned off its main viewport starts at (0, 0), so a position handed over in screen space is
	/// off by the origin of the window and hits something else entirely. The lParam of the mouse
	/// messages below already is a client position, hence it is passed on as it comes.
	auto GetCursorPos = [](LPARAM lParam) -> Math::Vector2 {
		return Math::Vector2(static_cast<float>(GET_X_LPARAM(lParam)), static_cast<float>(GET_Y_LPARAM(lParam)));
	};

	/// The two messages that carry a *screen* position instead, which has to be brought back into
	/// that same space before it goes anywhere.
	auto GetScreenCursorPos = [&hWnd](LPARAM lParam) -> Math::Vector2 {
		POINT CursorPos
		{
			GET_X_LPARAM(lParam),
			GET_Y_LPARAM(lParam)
		};
		::ScreenToClient(hWnd, &CursorPos);

		return Math::Vector2(static_cast<float>(CursorPos.x), static_cast<float>(CursorPos.y));
	};

	switch (Msg)
	{
	case WM_ACTIVATE:
		break;
	case WM_SIZE:
	{
		/// Nothing else keeps Width/Height in sync with the real client area, and everything that
		/// has to follow it (the swap chain, and through it the UI) reads them off the window.
		/// SIZE_MINIMIZED carries a 0x0 client area, which is not a size worth rendering at.
		if (wParam != SIZE_MINIMIZED && LOWORD(lParam) != 0 && HIWORD(lParam) != 0)
		{
			const uint32_t Width = static_cast<uint32_t>(LOWORD(lParam));
			const uint32_t Height = static_cast<uint32_t>(HIWORD(lParam));

			if (m_Window)
			{
				m_Window->UpdateSize(Width, Height);
			}

			DispatchWindowResizedMessage(Width, Height);
		}
		break;
	}
	case WM_DPICHANGED:
	{
		/// Sent when the window lands on a monitor of another density. Windows already scaled the
		/// non-client area for us and proposes a rect that keeps the whole window - client area
		/// included - the same physical size; ignoring it leaves the UI laid out for the old one.
		const ::RECT* SuggestedRect = reinterpret_cast<const ::RECT*>(lParam);
		if (SuggestedRect)
		{
			::SetWindowPos(hWnd, nullptr,
				SuggestedRect->left, SuggestedRect->top,
				SuggestedRect->right - SuggestedRect->left,
				SuggestedRect->bottom - SuggestedRect->top,
				SWP_NOZORDER | SWP_NOACTIVATE);
		}
		break;
	}
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

		/// WM_MOUSEWHEEL (and WM_NCMOUSEMOVE below) is the exception: its lParam holds a screen
		/// position, unlike every other mouse message.
		DispatchMouseWheelMessage(WheelDelta * s_SpinFactor, GetScreenCursorPos(lParam));
		break;
	}
	case WM_MOUSEMOVE:
		DispatchMouseMoveMessage(GetCursorPos(lParam));
		break;
	case WM_NCMOUSEMOVE:
		DispatchMouseMoveMessage(GetScreenCursorPos(lParam));
		break;
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
	{
		auto ActualKeyCode = MapKeyCode(wParam, lParam);
		DispatchKeyDownMessage(KeyEvent{ ActualKeyCode, wParam });
		LOG_DEBUG(LogDefault, "Key \"{}\" down", magic_enum::enum_name(ActualKeyCode).data());
		break;
	}
	case WM_KEYUP:
	case WM_SYSKEYUP:
	{
		auto ActualKeyCode = MapKeyCode(wParam, lParam);
		DispatchKeyUpMessage(KeyEvent{ ActualKeyCode, wParam });
		LOG_DEBUG(LogDefault, "Key \"{}\" up", magic_enum::enum_name(ActualKeyCode).data());
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
		/// from_wide() counts on a terminated range: handing it the address of a lone wchar_t makes
		/// it read past that variable for the length, which is undefined behaviour.
		const wchar_t WChar[] { static_cast<wchar_t>(wParam), L'\0' };
		const auto Char = string::from_wide(std::wstring_view(WChar, 1u));
		if (!Char.empty())
		{
			DispatchKeyCharMessage(Char[0]);
		}
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
