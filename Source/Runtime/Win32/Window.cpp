#include "Window.h"
#include "System.h"
#include "Application/Win32/Resource.h"
#include "Application/BaseApplication.h"
#include "Services/SpdLogService.h"

#if PLATFORM_WIN32

#include <Windows.h>
#include <windowsx.h>
#include <shlobj.h>

class MessageProcessor
{
public:
	MessageProcessor(Window& InWindow)
		: m_Window(InWindow)
	{
	}

	void ProcessMessage(uint32_t Message, ::WPARAM WParam, ::LPARAM LParam)
	{
		switch (Message)
		{
		case WM_ACTIVATE:
			m_Window.SetStatus(WParam == WA_INACTIVE ? EWindowStatus::Inactivate : EWindowStatus::Activate);
			EventRouter::Get().DispatchWindowStatusChanged((LOWORD(WParam) == WA_INACTIVE) ? EWindowStatus::Inactivate : EWindowStatus::Activate);
			break;
		case WM_SIZE:
			if (SIZE_RESTORED == WParam)
			{
				m_Window.SetStatus(EWindowStatus::Activate);
				EventRouter::Get().DispatchWindowStatusChanged(EWindowStatus::Activate);
			}
			else if (SIZE_MINIMIZED == WParam)
			{
				m_Window.SetStatus(EWindowStatus::Inactivate);
				EventRouter::Get().DispatchWindowStatusChanged(EWindowStatus::Inactivate);
			}
			break;
		case WM_ENTERSIZEMOVE:
			m_Window.SetStatus(EWindowStatus::Inactivate);
			EventRouter::Get().DispatchWindowStatusChanged(EWindowStatus::Inactivate);
			break;
		case WM_EXITSIZEMOVE:
			m_Window.SetStatus(EWindowStatus::Activate);
			m_Window.UpdateSize();
			EventRouter::Get().DispatchWindowStatusChanged(EWindowStatus::Activate);
			break;
		case WM_QUIT:
		case WM_DESTROY:
			m_Window.SetStatus(EWindowStatus::Destroyed);
			EventRouter::Get().DispatchWindowStatusChanged(EWindowStatus::Destroyed);
			::PostQuitMessage(0);
			break;
		case WM_NCLBUTTONDBLCLK:
			m_Window.UpdateSize();
			break;
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_LBUTTONDBLCLK:
		{
			MouseButtonEvent Event
			{
				EMouseButton::LButton,
				Message == WM_LBUTTONDOWN ? EKeyState::Down : Message == WM_LBUTTONDBLCLK ? EKeyState::DoubleClick : EKeyState::Up
			};
			EventRouter::Get().DispatchMouseButtonEvent(Event);
		}
		break;
		case WM_RBUTTONDOWN:
		case WM_RBUTTONDBLCLK:
		case WM_RBUTTONUP:
		{
			MouseButtonEvent Event
			{
				EMouseButton::RButton,
				Message == WM_RBUTTONDOWN ? EKeyState::Down : Message == WM_RBUTTONDBLCLK ? EKeyState::DoubleClick : EKeyState::Up,
			};
			EventRouter::Get().DispatchMouseButtonEvent(Event);
		}
		break;
		case WM_MBUTTONDOWN:
		case WM_MBUTTONUP:
		case WM_MBUTTONDBLCLK:
		{
			MouseButtonEvent Event
			{
				EMouseButton::MButton,
				Message == WM_MBUTTONDOWN ? EKeyState::Down : Message == WM_MBUTTONDOWN ? EKeyState::DoubleClick : EKeyState::Up,
			};
			EventRouter::Get().DispatchMouseButtonEvent(Event);
		}
		break;
		case WM_MOUSEWHEEL:
		{
			static const float s_WheelDelta = 1.0f / WHEEL_DELTA;
			MouseWheelEvent Event
			{
				static_cast<int16_t>(GET_WHEEL_DELTA_WPARAM(WParam)) * s_WheelDelta
			};
			EventRouter::Get().DispatchMouseWheelEvent(Event);
		}
		break;
		case WM_MOUSEMOVE:
		{
			MouseMoveEvent Event
			{
				static_cast<float>(GET_X_LPARAM(LParam)),
				static_cast<float>(GET_Y_LPARAM(LParam))
			};
			EventRouter::Get().DispatchMouseMoveEvent(Event);
		}
		break;
		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
		case WM_KEYUP:
		case WM_SYSKEYUP:
		{
			KeyEvent Event
			{
				EKeyboardKey::None,
				Message == WM_KEYDOWN ? EKeyState::Down : EKeyState::Up
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
					if (::MapVirtualKeyA((LParam & 0x00ff0000) >> 16, MAPVK_VK_TO_VSC) == VK_LSHIFT)
					{
						Event.Modifiers.LeftShift = true;
					}
					else
					{
						Event.Modifiers.RightShift = true;
					}
					Event.Key = EKeyboardKey::Shift;
					break;
				case VK_CONTROL:
					if ((LParam & 0x1000000) == 0)
					{
						Event.Modifiers.LeftCtrl = true;
					}
					else
					{
						Event.Modifiers.RightCtrl = true;
					}
					Event.Key = EKeyboardKey::Ctrl;
					break;
				case VK_MENU:
					if ((LParam & 0x1000000) == 0)
					{
						Event.Modifiers.LeftAlt = true;
					}
					else
					{
						Event.Modifiers.RightAlt = true;
					}
					Event.Key = EKeyboardKey::Alt;
					break;
				case VK_CAPITAL:
					Event.Key = EKeyboardKey::CapsLock;
					Event.Modifiers.CapsLock = (::GetKeyState(VK_CAPITAL) & 0x0001) != 0;
					break;
				case VK_ESCAPE:
					Event.Key = EKeyboardKey::Esc;
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
				case VK_F2:
					Event.Key = EKeyboardKey::F2;
					break;
				case VK_F3:
					Event.Key = EKeyboardKey::F3;
					break;
				case VK_F4:
					Event.Key = EKeyboardKey::F4;
					break;
				case VK_F5:
					Event.Key = EKeyboardKey::F5;
					break;
				case VK_F6:
					Event.Key = EKeyboardKey::F6;
					break;
				case VK_F7:
					Event.Key = EKeyboardKey::F7;
					break;
				case VK_F8:
					Event.Key = EKeyboardKey::F8;
					break;
				case VK_F9:
					Event.Key = EKeyboardKey::F9;
					break;
				case VK_F10:
					Event.Key = EKeyboardKey::F10;
					break;
				case VK_F11:
					Event.Key = EKeyboardKey::F11;
					break;
				case VK_F12:
					Event.Key = EKeyboardKey::F12;
					break;
				default:
					Event.Key = EKeyboardKey::Other;
					break;
				}
			}
			EventRouter::Get().DispatchKeyEvent(Event);
		}
		break;
		case WM_GETMINMAXINFO:
		{
			::LPMINMAXINFO MinMaxInfo = reinterpret_cast<::LPMINMAXINFO>(LParam);
			assert(MinMaxInfo);
			MinMaxInfo->ptMinTrackSize =
			{
				static_cast<long>(m_Window.GetMinWidth()),
				static_cast<long>(m_Window.GetMinHeight())
			};
		}
		break;
		case WM_CHAR:
		{
			CharEvent Event
			{
				static_cast<char>(WParam)
			};
			EventRouter::Get().DispatchCharEvent(Event);
		}
		break;
		}
	}
private:
	Window& m_Window;
};

static ::LRESULT MessageProc(::HWND HWnd, uint32_t Message, ::WPARAM WParam, ::LPARAM LParam)
{
	if (Message == WM_CREATE)
	{
		::CREATESTRUCTA* CreateStruct = reinterpret_cast<::CREATESTRUCTA*>(LParam);
		assert(CreateStruct);
		VERIFY_WITH_PLATFORM_MESSAGE(::SetWindowLongPtrA(HWnd, 0, reinterpret_cast<LONG_PTR>(CreateStruct->lpCreateParams)) == 0);
	}
	else
	{
		if (auto Processor = reinterpret_cast<MessageProcessor*>(::GetWindowLongPtrA(HWnd, 0)))
		{
			Processor->ProcessMessage(Message, WParam, LParam);
		}
	}

	return ::DefWindowProcA(HWnd, Message, WParam, LParam);
}

Window::Window(const WindowSettings& Settings)
	: m_MinWidth(std::max<uint32_t>(Settings.MinWidth, MINIMAL_WINDOW_SIZE))
	, m_MinHeight(std::max<uint32_t>(Settings.MinHeight, MINIMAL_WINDOW_SIZE))
	, m_Width(std::max<uint32_t>(Settings.Width, m_MinWidth))
	, m_Height(std::max<uint32_t>(Settings.Height, m_MinHeight))
	, m_Mode(Settings.Mode)
	, m_Status(EWindowStatus::Activate)
	, m_Handle(nullptr)
{
	::HINSTANCE HInstance = reinterpret_cast<::HINSTANCE>(System::GetApplicationInstance());
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
		Settings.Title.c_str(),
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

	m_MessageProcessor = std::make_shared<MessageProcessor>(*this);

	uint32_t ExtraWindowStyle = 0u;
	::HWND Handle = ::CreateWindowExA(
		0,
		Settings.Title.c_str(),
		Settings.Title.c_str(),
		WS_OVERLAPPEDWINDOW ^ ExtraWindowStyle,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		Rect.right - Rect.left,
		Rect.bottom - Rect.top,
		nullptr,
		nullptr,
		HInstance,
		reinterpret_cast<void*>(m_MessageProcessor.get()));
	VERIFY_WITH_PLATFORM_MESSAGE(Handle);

	::ShowWindow(Handle, SW_SHOWDEFAULT);
	VERIFY_WITH_PLATFORM_MESSAGE(::UpdateWindow(Handle) != 0);

	m_Handle = reinterpret_cast<void*>(Handle);

	SetMode(Settings.Mode);
}

void Window::UpdateSize()
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

		EventRouter::Get().DispatchWindowResized(m_Width, m_Height);
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

#endif // PLATFORM_WIN32
