#include "Core/Window.h"
#include "Core/PlatformMisc.h"
#include "Engine/Application/Resource.h"
#include "Engine/Services/SpdLogService.h"

#if defined(PLATFORM_WIN32)
#include <Windows.h>
#include <windowsx.h>
#include <shlobj.h>

static ::LRESULT MessageProc(::HWND HWnd, uint32_t Message, ::WPARAM WParam, ::LPARAM LParam)
{
	if (Message == WM_CREATE)
	{
		::CREATESTRUCTA* CreateStruct = reinterpret_cast<::CREATESTRUCTA*>(LParam);
		assert(CreateStruct);
		VERIFY_WITH_PLATFORM_MESSAGE(::SetWindowLongPtrA(HWnd, 0, reinterpret_cast<LONG_PTR>(CreateStruct->lpCreateParams)) == 0);
	}

	if (auto Win32Window = reinterpret_cast<Window*>(::GetWindowLongPtrA(HWnd, 0)))
	{
		Win32Window->ProcessMessage(Message, WParam, LParam);
	}

	return ::DefWindowProcA(HWnd, Message, WParam, LParam);
}

Window::Window(const WindowCreateInfo& CreateInfo)
	: m_MinWidth(std::max<uint32_t>(CreateInfo.MinWidth, MINIMAL_WINDOW_SIZE))
	, m_MinHeight(std::max<uint32_t>(CreateInfo.MinHeight, MINIMAL_WINDOW_SIZE))
	, m_Width(std::max<uint32_t>(CreateInfo.Width, CreateInfo.MinWidth))
	, m_Height(std::max<uint32_t>(CreateInfo.Height, CreateInfo.MinHeight))
{
	::HINSTANCE HInstance = reinterpret_cast<::HINSTANCE>(PlatformMisc::GetCurrentModuleHandle());
	assert(HInstance);

	::HICON Icon = ::LoadIcon(HInstance, MAKEINTRESOURCE(ICON_NVIDIA));
	VERIFY_WITH_PLATFORM_MESSAGE(Icon);

	::WNDCLASSEXA WndClassEx
	{
		sizeof(::WNDCLASSEXA),
		CS_HREDRAW | CS_VREDRAW,
		MessageProc,
		0,
		sizeof(void*),
		HInstance,
		Icon,
		::LoadCursor(0, IDC_ARROW),
		static_cast<::HBRUSH>(::GetStockObject(BLACK_BRUSH)),
		nullptr,
		CreateInfo.Title.c_str(),
		Icon
	};
	VERIFY_WITH_PLATFORM_MESSAGE(::RegisterClassExA(&WndClassEx) != 0);

	::RECT Rect
	{ 
		0l, 
		0l, 
		static_cast<long>(m_Width), 
		static_cast<long>(m_Height) 
	};
	VERIFY_WITH_PLATFORM_MESSAGE(::AdjustWindowRect(&Rect, WS_OVERLAPPEDWINDOW, false) != 0);

	uint32_t ExtraWindowStyle = 0u;
	::HWND Handle = ::CreateWindowA(
		CreateInfo.Title.c_str(),
		CreateInfo.Title.c_str(),
		WS_OVERLAPPEDWINDOW ^ ExtraWindowStyle,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		Rect.right - Rect.left,
		Rect.bottom - Rect.top,
		nullptr,
		nullptr,
		HInstance,
		static_cast<void*>(this));
	VERIFY_WITH_PLATFORM_MESSAGE(Handle);

	::ShowWindow(Handle, SW_SHOWDEFAULT);
	VERIFY_WITH_PLATFORM_MESSAGE(::UpdateWindow(Handle) != 0);

	m_Handle = reinterpret_cast<void*>(Handle);
}

void Window::UpdateSize()
{
	if (m_Handle)
	{
		::RECT Rect;
		VERIFY_WITH_PLATFORM_MESSAGE(::GetClientRect(reinterpret_cast<::HWND>(m_Handle), &Rect) != 0);
		uint32_t Width = static_cast<uint32_t>(Rect.right - Rect.left);
		uint32_t Height = static_cast<uint32_t>(Rect.bottom - Rect.top);

		if (m_Width != Width || m_Height != Height)
		{
			LOG_DEBUG("System: Window resized, width = {}, height = {}", m_Width, m_Height);
			m_Width = Width;
			m_Height = Height;

			MessageRouter::Get().DispatchWindowResized(m_Width, m_Height);
		}
	}
}

void Window::SetMode(EWindowMode Mode)
{
	if (m_Mode == Mode)
	{
		return;
	}

	m_Mode = Mode;

	::HWND Handle = reinterpret_cast<::HWND>(m_Handle);
	const ::LONG WindowedStyle = WS_OVERLAPPEDWINDOW;
	const ::LONG FullscreenStyle = WS_POPUP;
	::LONG WindowStyle = ::GetWindowLong(Handle, GWL_STYLE);
	::UINT Flags = SWP_NOMOVE | SWP_NOZORDER | SWP_FRAMECHANGED;
	
	::RECT WindowRect;
	::GetClientRect(Handle, &WindowRect);

	const int32_t Left = WindowRect.left;
	const int32_t Top = WindowRect.top;
	int32_t Width = 0, Height = 0;

	if (Mode == EWindowMode::Windowed)
	{
		WindowStyle &= ~FullscreenStyle;
		WindowStyle |= WindowedStyle;
		Flags |= SWP_NOSIZE;
	}
	else if (Mode == EWindowMode::BorderlessFullscreen || Mode == EWindowMode::ExclusiveFullscreen)
	{
		WindowStyle &= ~WindowedStyle;
		WindowStyle |= FullscreenStyle;

		::HMONITOR Monitor = ::MonitorFromWindow(Handle, Mode == EWindowMode::ExclusiveFullscreen ? MONITOR_DEFAULTTOPRIMARY : MONITOR_DEFAULTTONEAREST);
		::MONITORINFO MonitorInfo;
		MonitorInfo.cbSize = sizeof(::MONITORINFO);
		::GetMonitorInfo(Monitor, &MonitorInfo);

		Width = static_cast<int32_t>(MonitorInfo.rcMonitor.right - MonitorInfo.rcMonitor.left);
		Height = static_cast<int32_t>(MonitorInfo.rcMonitor.bottom - MonitorInfo.rcMonitor.top);
	}

	::SetWindowLong(Handle, GWL_STYLE, WindowStyle);
	::SetWindowPos(Handle, nullptr, Left, Top, Width, Height, Flags);

	UpdateSize();
}

void Window::ProcessMessage(uint32_t Message, size_t WParam, intptr_t LParam)
{
	switch (Message)
	{
	case WM_CREATE:
		break;
	case WM_ACTIVATE:
		MessageRouter::Get().DispatchWindowStatusChanged((LOWORD(WParam) == WA_INACTIVE) ? EWindowStatus::Inactivate : EWindowStatus::Activate);
		break;
	case WM_SIZE:
		if (SIZE_RESTORED == WParam)
		{
			MessageRouter::Get().DispatchWindowStatusChanged(EWindowStatus::Activate);
		}
		else if (SIZE_MINIMIZED == WParam)
		{
			MessageRouter::Get().DispatchWindowStatusChanged(EWindowStatus::Inactivate);
		}
		break;
	case WM_ENTERSIZEMOVE:
		MessageRouter::Get().DispatchWindowStatusChanged(EWindowStatus::Inactivate);
		break;
	case WM_EXITSIZEMOVE:
		MessageRouter::Get().DispatchWindowStatusChanged(EWindowStatus::Activate);
		UpdateSize();
		break;
	case WM_QUIT:
	case WM_DESTROY:
		MessageRouter::Get().DispatchWindowStatusChanged(EWindowStatus::Destroyed);
		::PostQuitMessage(0);
		break;
	case WM_NCLBUTTONDBLCLK:
		UpdateSize();
		break;
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_LBUTTONDBLCLK:
	{
		MouseEvent Event
		{
			EMouseButton::LButton,
			false,
			0,
			Message == WM_LBUTTONDOWN ? EKeyState::Down : Message == WM_LBUTTONDBLCLK ? EKeyState::DoubleClick : EKeyState::Up,
			{ static_cast<float>(GET_X_LPARAM(LParam)), static_cast<float>(GET_Y_LPARAM(LParam)) },
		};
		MessageRouter::Get().DispatchMouseEvent(Event);
	}
	break;
	case WM_RBUTTONDOWN:
	case WM_RBUTTONDBLCLK:
	case WM_RBUTTONUP:
	{
		MouseEvent Event
		{
			EMouseButton::RButton,
			false,
			0,
			Message == WM_RBUTTONDOWN ? EKeyState::Down : Message == WM_RBUTTONDBLCLK ? EKeyState::DoubleClick : EKeyState::Up,
			{ static_cast<float>(GET_X_LPARAM(LParam)), static_cast<float>(GET_Y_LPARAM(LParam)) }
		};
		MessageRouter::Get().DispatchMouseEvent(Event);
	}
	break;
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_MBUTTONDBLCLK:
	{
		MouseEvent Event
		{
			EMouseButton::MButton,
			false,
			0,
			Message == WM_MBUTTONDOWN ? EKeyState::Down : Message == WM_MBUTTONDOWN ? EKeyState::DoubleClick : EKeyState::Up,
			{ static_cast<float>(GET_X_LPARAM(LParam)), static_cast<float>(GET_Y_LPARAM(LParam)) }
		};
		MessageRouter::Get().DispatchMouseEvent(Event);
	}
	break;
	case WM_MOUSEWHEEL:
	{
		MouseEvent Event
		{
			EMouseButton::None,
			false,
			GET_WHEEL_DELTA_WPARAM(WParam),
			EKeyState::None,
			{ 0.0f, 0.0f}
		};
		MessageRouter::Get().DispatchMouseEvent(Event);
	}
	break;
	case WM_MOUSEMOVE:
	{
		MouseEvent Event
		{
			EMouseButton::None,
			true,
			0,
			EKeyState::None,
			{ static_cast<float>(GET_X_LPARAM(LParam)), static_cast<float>(GET_Y_LPARAM(LParam)) }
		};
		MessageRouter::Get().DispatchMouseEvent(Event);
	}
	break;
	case WM_KEYDOWN:
	case WM_KEYUP:
	{
		KeyboardEvent Event
		{
			EKeyboardKey::None,
			Message == WM_KEYDOWN ? EKeyState::Down : EKeyState::Up,
			0
		};

		if ((WParam >= '0' && WParam <= '9') || (WParam >= 'A' && WParam <= 'Z'))
		{
			Event.Key = static_cast<EKeyboardKey>(WParam);
		}
		else
		{
			switch (WParam)
			{
			case VK_TAB:
				Event.Key = EKeyboardKey::Tab;
				break;
			case VK_RETURN:
				Event.Key = EKeyboardKey::Enter;
				break;
			case VK_SHIFT:
				Event.Key = EKeyboardKey::Shift;
				break;
			case VK_CONTROL:
				Event.Key = EKeyboardKey::Ctrl;
				break;
			case VK_MENU:
				Event.Key = EKeyboardKey::Alt;
				break;
			case VK_ESCAPE:
				Event.Key = EKeyboardKey::Escape;
				break;
			case VK_SPACE:
				Event.Key = EKeyboardKey::Space;
				break;
			case VK_PRIOR:
				Event.Key = EKeyboardKey::PageUp;
				break;
			case VK_NEXT:
				Event.Key = EKeyboardKey::PageDown;
				break;
			case VK_HOME:
				Event.Key = EKeyboardKey::Home;
				break;
			case VK_LEFT:
				Event.Key = EKeyboardKey::Left;
				break;
			case VK_UP:
				Event.Key = EKeyboardKey::Up;
				break;
			case VK_RIGHT:
				Event.Key = EKeyboardKey::Right;
				break;
			case VK_DOWN:
				Event.Key = EKeyboardKey::Down;
				break;
			case VK_F1:
				Event.Key = EKeyboardKey::F1;
				break;
			default:
				Event.Key = EKeyboardKey::Other;
				break;
			}
		}
		MessageRouter::Get().DispatchKeyboardEvent(Event);
	}
	break;
	case WM_SYSKEYDOWN:
	{
		if (WParam == VK_RETURN && LParam & (1 << 29)) /// Enter + Shift
		{

		}
		//DispatchKeyboardEvent();
	}
	break;
	case WM_GETMINMAXINFO:
	{
		::LPMINMAXINFO MinMaxInfo = reinterpret_cast<::LPMINMAXINFO>(LParam);
		assert(MinMaxInfo);
		MinMaxInfo->ptMinTrackSize =
		{
			static_cast<long>(m_MinWidth),
			static_cast<long>(m_MinHeight)
		};
	}
	break;
	case WM_CHAR:
	{
		KeyboardEvent Event
		{
			EKeyboardKey::None,
			Message == WM_KEYDOWN ? EKeyState::Down : EKeyState::Up,
			static_cast<char>(WParam)
		};
		MessageRouter::Get().DispatchKeyboardEvent(Event);
	}
	break;
	}
}

void Window::PumpMessages()
{
	::MSG Message;
	if (::PeekMessageA(&Message, nullptr, 0u, 0u, PM_REMOVE))
	{
		::TranslateMessage(&Message);
		::DispatchMessageA(&Message);
	}
}

#else
	#error Unknown platform!
#endif

