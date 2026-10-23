
#include "Application/Windows/WindowsApplication.h"

#if PLATFORM_WINDOWS

#include "Application/Windows/WindowsWindow.h"
#include "Application/Windows/Resource.h"
#include "Core/SpdLogging.h"
#include <windowsx.h>

void LogKeyEvent(EKeyboardKey Key, const KeyModifiers& Modifiers, uint32_t Msg)
{
	string KeyState = (Msg == WM_KEYDOWN || Msg == WM_SYSKEYDOWN) ? "down" : "up";
	string Modifier;

	static const std::vector<string> KeyNames
	{
		"LeftCtrl",
		"LeftShift",
		"LeftAlt",
		"LeftMenu",
		"RightCtrl",
		"RightShift",
		"RightAlt",
		"RightMenu",
		"CapsLock"
	};
	const std::vector<bool> KeyEnabled
	{
		Modifiers.LeftCtrl,
		Modifiers.LeftShift,
		Modifiers.LeftAlt,
		Modifiers.LeftMenu,
		Modifiers.RightCtrl,
		Modifiers.RightShift,
		Modifiers.RightAlt,
		Modifiers.RightMenu,
		Modifiers.CapsLock
	};

	for (uint32_t Index = 0u; Index < KeyEnabled.size(); ++Index)
	{
		if (KeyEnabled[Index])
		{
			Modifier += "+";
			Modifier += KeyNames[Index];
		}
	}

	LOG_INFO(LogDefault, "Key \"{}{}\" {}", magic_enum::enum_name(Key).data(), Modifier, KeyState);
}

WindowsApplication::WindowsApplication()
	: Application()
{
	RegisterClass();
}

void WindowsApplication::RegisterClass()
{
	HINSTANCE HInstance = reinterpret_cast<HINSTANCE>(GetHandle());
	VERIFY_WITH_SYSTEM_MESSAGE(HInstance);

	HICON HIcon = ::LoadIconW(HInstance, MAKEINTRESOURCEW(ICON_NVIDIA));
	VERIFY_WITH_SYSTEM_MESSAGE(HIcon);

	WNDCLASSEXW WndClassEx
	{
		sizeof(WNDCLASSEXW),
		CS_HREDRAW | CS_VREDRAW,
		ApplicationMessageProc,
		0,
		sizeof(void*),
		HInstance,
		HIcon,
		::LoadCursorW(0, IDC_ARROW),
		static_cast<HBRUSH>(::GetStockObject(DKGRAY_BRUSH)),
		nullptr,
		WindowsWindow::WindowClassName.data(),
		HIcon
	};
	VERIFY_WITH_SYSTEM_MESSAGE(::RegisterClassExW(&WndClassEx) != 0);
}

void WindowsApplication::PumpMessages()
{
	::MSG Message;

	/// Drained instead of a single message per frame: a key press alone already arrives as three of
	/// them (down, char, up), and dropping to one per frame makes input lag behind the presentation,
	/// which stalls waiting for the queue on top of that.
	while (::PeekMessageW(&Message, nullptr, 0u, 0u, PM_REMOVE))
	{
		::TranslateMessage(&Message);
		::DispatchMessageW(&Message);
	}
}

std::shared_ptr<Window> WindowsApplication::CreateWindow(uint32_t Width, uint32_t Height, const char* Title)
{
	return std::make_shared<WindowsWindow>(reinterpret_cast<HINSTANCE>(GetHandle()), Width, Height, Title);
}

LRESULT WindowsApplication::ApplicationMessageProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	return Cast<WindowsApplication>(Application::Get()).MessageProc(hWnd, Msg, wParam, lParam);
}

LRESULT WindowsApplication::MessageProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	/// Everything downstream - ImGui above all - works in client coordinates: with multi viewports
	/// turned off its main viewport starts at (0, 0), so a position handed over in screen space is
	/// off by the origin of the window and hits something else entirely. The lParam of the mouse
	/// messages below already is a client position, hence it is passed on as it comes.
	static auto GetCursorPos = [](LPARAM lParam) -> Math::Vector2 {
		return Math::Vector2(static_cast<float>(GET_X_LPARAM(lParam)), static_cast<float>(GET_Y_LPARAM(lParam)));
	};

	/// The two messages that carry a *screen* position instead, which has to be brought back into
	/// that same space before it goes anywhere.
	static auto GetScreenCursorPos = [&hWnd](LPARAM lParam) -> Math::Vector2 {
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

			if (auto MainWindow = GetMainWindow())
			{
				MainWindow->OnResize(Width, Height);
			}

			DispatchMainWindowResizedMessage(Width, Height);
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
		SetStatus(EStatus::Quit);
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
		KeyModifiers Modifiers;
		auto KeyboardKey = DecodeKeyboardKey(wParam, lParam, Modifiers);
		DispatchKeyDownMessage(KeyEvent{ KeyboardKey, wParam }, Modifiers);
		LogKeyEvent(KeyboardKey, Modifiers, Msg);
		break;
	}
	case WM_KEYUP:
	case WM_SYSKEYUP:
	{
		KeyModifiers Modifiers;
		auto KeyboardKey = DecodeKeyboardKey(wParam, lParam, Modifiers);
		DispatchKeyUpMessage(KeyEvent{ KeyboardKey, wParam }, Modifiers);
		LogKeyEvent(KeyboardKey, Modifiers, Msg);
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

EKeyboardKey WindowsApplication::DecodeKeyboardKey(uint64_t KeyCode, int64_t Mask, KeyModifiers& Modifiers)
{
	EKeyboardKey KeyboardKey = EKeyboardKey::None;

	switch (KeyCode)
	{
	case VK_ESCAPE:
		KeyboardKey = EKeyboardKey::Esc;
		break;
	case VK_TAB:
		KeyboardKey = EKeyboardKey::Tab;
		break;
	case VK_RETURN:
		KeyboardKey = EKeyboardKey::Enter;
		break;
	case VK_SHIFT:
		if ((Mask & 0x1000000) == 0)
		{
			KeyboardKey = EKeyboardKey::LeftShift;
			Modifiers.LeftShift = true;
		}
		else
		{
			KeyboardKey = EKeyboardKey::RightShift;
			Modifiers.RightShift = true;
		}
		break;
	case VK_CONTROL:
		if ((Mask & 0x1000000) == 0)
		{
			KeyboardKey = EKeyboardKey::LeftCtrl;
			Modifiers.LeftCtrl = true;
		}
		else
		{
			KeyboardKey = EKeyboardKey::RightCtrl;
			Modifiers.RightCtrl = true;
		}
		break;
	case VK_MENU:
		if ((Mask & 0x1000000) == 0)
		{
			KeyboardKey = EKeyboardKey::LeftAlt;
			Modifiers.LeftAlt = true;
		}
		else
		{
			KeyboardKey = EKeyboardKey::RightAlt;
			Modifiers.RightAlt = true;
		}
		break;
	case VK_CAPITAL:
		KeyboardKey = EKeyboardKey::CapsLock;
		Modifiers.CapsLock = (::GetKeyState(VK_CAPITAL) & 0x0001) != 0;
		break;
	case VK_INSERT:
		KeyboardKey = EKeyboardKey::Insert;
		break;
	case VK_DELETE:
		KeyboardKey = EKeyboardKey::Delete;
		break;
	case VK_HOME:
		KeyboardKey = EKeyboardKey::Home;
		break;
	case VK_END:
		KeyboardKey = EKeyboardKey::End;
		break;
	case VK_PRIOR:
		KeyboardKey = EKeyboardKey::PageUp;
		break;
	case VK_NEXT:
		KeyboardKey = EKeyboardKey::PageDown;
		break;
	case VK_LEFT:
		KeyboardKey = EKeyboardKey::Left;
		break;
	case VK_UP:
		KeyboardKey = EKeyboardKey::Up;
		break;
	case VK_RIGHT:
		KeyboardKey = EKeyboardKey::Right;
		break;
	case VK_DOWN:
		KeyboardKey = EKeyboardKey::Down;
		break;
	case VK_F1:
		KeyboardKey = EKeyboardKey::F1;
		break;
	case VK_F2:
		KeyboardKey = EKeyboardKey::F2;
		break;
	case VK_F3:
		KeyboardKey = EKeyboardKey::F3;
		break;
	case VK_F4:
		KeyboardKey = EKeyboardKey::F4;
		break;
	case VK_F5:
		KeyboardKey = EKeyboardKey::F5;
		break;
	case VK_F6:
		KeyboardKey = EKeyboardKey::F6;
		break;
	case VK_F7:
		KeyboardKey = EKeyboardKey::F7;
		break;
	case VK_F8:
		KeyboardKey = EKeyboardKey::F8;
		break;
	case VK_F9:
		KeyboardKey = EKeyboardKey::F9;
		break;
	case VK_F10:
		KeyboardKey = EKeyboardKey::F10;
		break;
	case VK_F11:
		KeyboardKey = EKeyboardKey::F11;
		break;
	case VK_F12:
		KeyboardKey = EKeyboardKey::F12;
		break;
	case VK_SPACE:
		KeyboardKey = EKeyboardKey::Space;
		break;
	case VK_LWIN:
		KeyboardKey = EKeyboardKey::LeftMenu;
		break;
	case VK_RWIN:
		KeyboardKey = EKeyboardKey::RightMenu;
		break;
	case VK_BACK:
		KeyboardKey = EKeyboardKey::Backspace;
		break;
	default:
		if ((KeyCode >= '0' && KeyCode <= '9') || (KeyCode >= 'A' && KeyCode <= 'Z'))
		{
			KeyboardKey = static_cast<EKeyboardKey>(KeyCode);
		}
		else
		{
			KeyboardKey = EKeyboardKey::OEM;
		}
		break;
	}

	return KeyboardKey;
}

#endif
