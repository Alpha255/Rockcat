#include "Runtime/Core/Window.h"
#include "Runtime/Engine/Engine.h"
#include "Runtime/Core/PlatformMisc.h"
#include "Runtime/Engine/Application/Resource.h"

#if defined(PLATFORM_WIN32)
	#include <Windows.h>
	#include <windowsx.h>
	#include <shlobj.h>
#else
	#error Unknown platform!
#endif

#define MINIMAL_WINDOW_SIZE 32

static ::LRESULT MessageProc(::HWND HWnd, uint32_t Message, ::WPARAM WParam, ::LPARAM LParam)
{
	if (Message == WM_CREATE)
	{
		::CREATESTRUCTA* CreateStruct = reinterpret_cast<::CREATESTRUCTA*>(LParam);
		assert(CreateStruct);
		VERIFY_WITH_PLATFORM_MESSAGE(::SetWindowLongPtrA(HWnd, 0, reinterpret_cast<LONG_PTR>(CreateStruct->lpCreateParams)) == 0);
	}

	if (Window* Win32Window = reinterpret_cast<Window*>(::GetWindowLongPtrA(HWnd, 0)))
	{
		Win32Window->ProcessMessage(Message, WParam, LParam);
	}

	return ::DefWindowProcA(HWnd, Message, WParam, LParam);
}

Window::Window(const WindowCreateInfo& CreateInfo, IInputHandler* InputHandler)
	: m_MinWidth(std::max<uint32_t>(CreateInfo.MinWidth, MINIMAL_WINDOW_SIZE))
	, m_MinHeight(std::max<uint32_t>(CreateInfo.MinHeight, MINIMAL_WINDOW_SIZE))
	, m_Width(std::max<uint32_t>(CreateInfo.Width, CreateInfo.MinWidth))
	, m_Height(std::max<uint32_t>(CreateInfo.Height, CreateInfo.MinHeight))
	, m_InputHandler(InputHandler)
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
		static_cast<long32_t>(m_Width), 
		static_cast<long32_t>(m_Height) 
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

	m_Handle = reinterpret_cast<uint64_t>(Handle);
}

void Window::UpdateSize(bool8_t Signal)
{
	if (m_Handle)
	{
		::RECT Rect;
		VERIFY_WITH_PLATFORM_MESSAGE(::GetClientRect(reinterpret_cast<::HWND>(m_Handle), &Rect) != 0);
		m_Width = static_cast<uint32_t>(Rect.right - Rect.left);
		m_Height = static_cast<uint32_t>(Rect.bottom - Rect.top);
		LOG_DEBUG("System: Window resized, width = {}, height = {}", m_Width, m_Height);
	}

	if (Signal && m_InputHandler)
	{
		m_InputHandler->OnWindowResized(m_Width, m_Height);
	}
}

void Window::ProcessMessage(uint32_t Message, size_t WParam, intptr_t LParam)
{
	switch (Message)
	{
	case WM_CREATE:
		UpdateSize(true);
		break;
	case WM_ACTIVATE:
		if (LOWORD(WParam) == WA_INACTIVE)
		{
			m_State = EState::LostFocus;
		}
		else
		{
			m_State = EState::OnFocus;
		}
		break;
	case WM_SIZE:
		if (SIZE_RESTORED == WParam)
		{
			m_State = EState::OnFocus;
		}
		else if (SIZE_MINIMIZED == WParam)
		{
			m_State = EState::LostFocus;
		}
		/// updateSize();
		break;
	case WM_ENTERSIZEMOVE:
		m_State = EState::LostFocus;
		break;
	case WM_EXITSIZEMOVE:
		m_State = EState::OnFocus;
		UpdateSize(true);
		break;
	case WM_QUIT:
	case WM_DESTROY:
		m_State = EState::Destroyed;
		::PostQuitMessage(0);
		break;
	case WM_NCLBUTTONDBLCLK:
		UpdateSize(true);
		break;
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_LBUTTONDBLCLK:
		m_MouseEvent.WheelDelta = 0;
		m_MouseEvent.OnMove = false;
		m_MouseEvent.Button = EMouseButton::LButton;
		m_MouseEvent.State = Message == WM_LBUTTONDOWN ? EKeyState::Down : Message == WM_LBUTTONDBLCLK ? EKeyState::DBClick : EKeyState::Up;
		m_MouseEvent.Position.x = static_cast<float32_t>(GET_X_LPARAM(LParam));
		m_MouseEvent.Position.y = static_cast<float32_t>(GET_Y_LPARAM(LParam));
		OnMouseEvent();
		break;
	case WM_RBUTTONDOWN:
	case WM_RBUTTONDBLCLK:
	case WM_RBUTTONUP:
		m_MouseEvent.WheelDelta = 0;
		m_MouseEvent.OnMove = false;
		m_MouseEvent.Button = EMouseButton::RButton;
		m_MouseEvent.State = Message == WM_RBUTTONDOWN ? EKeyState::Down : Message == WM_RBUTTONDBLCLK ? EKeyState::DBClick : EKeyState::Up;
		m_MouseEvent.Position.x = static_cast<float32_t>(GET_X_LPARAM(LParam));
		m_MouseEvent.Position.y = static_cast<float32_t>(GET_Y_LPARAM(LParam));
		OnMouseEvent();
		break;
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_MBUTTONDBLCLK:
		m_MouseEvent.WheelDelta = 0;
		m_MouseEvent.OnMove = false;
		m_MouseEvent.Button = EMouseButton::MButton;
		m_MouseEvent.State = Message == WM_MBUTTONDOWN ? EKeyState::Down : Message == WM_MBUTTONDBLCLK ? EKeyState::DBClick : EKeyState::Up;
		m_MouseEvent.Position.x = static_cast<float32_t>(GET_X_LPARAM(LParam));
		m_MouseEvent.Position.y = static_cast<float32_t>(GET_Y_LPARAM(LParam));
		OnMouseEvent();
		break;
	case WM_MOUSEWHEEL:
		m_MouseEvent.State = EKeyState::Up;
		m_MouseEvent.OnMove = false;
		m_MouseEvent.WheelDelta = GET_WHEEL_DELTA_WPARAM(WParam) > 0 ? 1 : -1;
		OnMouseEvent();
		break;
	case WM_MOUSEMOVE:
		m_MouseEvent.OnMove = true;
		m_MouseEvent.WheelDelta = 0;
		m_MouseEvent.Button = EMouseButton::None;
		m_MouseEvent.State = EKeyState::Up;
		m_MouseEvent.Position.x = static_cast<float32_t>(GET_X_LPARAM(LParam));
		m_MouseEvent.Position.y = static_cast<float32_t>(GET_Y_LPARAM(LParam));
		OnMouseEvent();
		break;
	case WM_KEYDOWN:
	case WM_KEYUP:
		m_KeyboardEvent.State = Message == WM_KEYDOWN ? EKeyState::Down : EKeyState::Up;
		if ((WParam >= '0' && WParam <= '9') || (WParam >= 'A' && WParam <= 'Z'))
		{
			m_KeyboardEvent.Key = static_cast<EKeyboardKey>(WParam);
		}
		else
		{
			switch (WParam)
			{
			case VK_TAB:
				m_KeyboardEvent.Key = EKeyboardKey::Tab;
				break;
			case VK_RETURN:
				m_KeyboardEvent.Key = EKeyboardKey::Enter;
				break;
			case VK_SHIFT:
				m_KeyboardEvent.Key = EKeyboardKey::Shift;
				break;
			case VK_CONTROL:
				m_KeyboardEvent.Key = EKeyboardKey::Ctrl;
				break;
			case VK_MENU:
				m_KeyboardEvent.Key = EKeyboardKey::Alt;
				break;
			case VK_ESCAPE:
				m_KeyboardEvent.Key = EKeyboardKey::Escape;
				break;
			case VK_SPACE:
				m_KeyboardEvent.Key = EKeyboardKey::Space;
				break;
			case VK_PRIOR:
				m_KeyboardEvent.Key = EKeyboardKey::PageUp;
				break;
			case VK_NEXT:
				m_KeyboardEvent.Key = EKeyboardKey::PageDown;
				break;
			case VK_HOME:
				m_KeyboardEvent.Key = EKeyboardKey::Home;
				break;
			case VK_LEFT:
				m_KeyboardEvent.Key = EKeyboardKey::Left;
				break;
			case VK_UP:
				m_KeyboardEvent.Key = EKeyboardKey::Up;
				break;
			case VK_RIGHT:
				m_KeyboardEvent.Key = EKeyboardKey::Right;
				break;
			case VK_DOWN:
				m_KeyboardEvent.Key = EKeyboardKey::Down;
				break;
			case VK_F1:
				m_KeyboardEvent.Key = EKeyboardKey::F1;
				break;
			default:
				m_KeyboardEvent.Key = EKeyboardKey::Other;
				break;
			}
		}
		OnKeyboardEvent();
		break;
	case WM_SYSKEYDOWN:
		if (WParam == VK_RETURN && LParam & (1 << 29)) /// Enter + Shift
		{
			UpdateSize(true);
		}
		
		OnKeyboardEvent();
		break;
	case WM_GETMINMAXINFO:
	{
		::LPMINMAXINFO MinMaxInfo = reinterpret_cast<::LPMINMAXINFO>(LParam);
		assert(MinMaxInfo);
		MinMaxInfo->ptMinTrackSize = 
		{ 
			static_cast<long32_t>(m_MinWidth), 
			static_cast<long32_t>(m_MinHeight) 
		};
	}
		break;
	case WM_CHAR:
		m_KeyboardEvent.InputChar = static_cast<char8_t>(WParam);
		OnKeyboardEvent();
		break;
	}
}

void Window::PollMessage()
{
	::MSG Message;
	if (::PeekMessageA(&Message, nullptr, 0u, 0u, PM_REMOVE))
	{
		::TranslateMessage(&Message);
		::DispatchMessageA(&Message);
	}
}

