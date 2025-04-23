#include "Application/BaseApplication.h"
#include "Application/ApplicationConfiguration.h"
#include "System.h"

#include <Windows.h>
#include <windowsx.h>
#include <shlobj.h>

void BaseApplication::ProcessMessage(uint32_t Message, size_t WParam, intptr_t LParam)
{
	switch (Message)
	{
	case WM_CREATE:
		break;
	case WM_ACTIVATE:
		m_Activate = (LOWORD(WParam) == WA_INACTIVE);
		MessageRouter::Get().DispatchWindowStatusChanged((LOWORD(WParam) == WA_INACTIVE) ? EWindowStatus::Inactivate : EWindowStatus::Activate);
		break;
	case WM_SIZE:
		if (SIZE_RESTORED == WParam)
		{
			m_Activate = true;
			MessageRouter::Get().DispatchWindowStatusChanged(EWindowStatus::Activate);
		}
		else if (SIZE_MINIMIZED == WParam)
		{
			m_Activate = false;
			MessageRouter::Get().DispatchWindowStatusChanged(EWindowStatus::Inactivate);
		}
		break;
	case WM_ENTERSIZEMOVE:
		m_Activate = false;
		MessageRouter::Get().DispatchWindowStatusChanged(EWindowStatus::Inactivate);
		break;
	case WM_EXITSIZEMOVE:
		m_Activate = true;
		m_Window->UpdateSize();
		MessageRouter::Get().DispatchWindowStatusChanged(EWindowStatus::Activate);
		break;
	case WM_QUIT:
	case WM_DESTROY:
		MessageRouter::Get().DispatchWindowStatusChanged(EWindowStatus::Destroyed);
		::PostQuitMessage(0);
		break;
	case WM_NCLBUTTONDBLCLK:
		m_Window->UpdateSize();
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
			static_cast<long>(GetConfigs().WindowDesc.MinWidth),
			static_cast<long>(GetConfigs().WindowDesc.MinHeight)
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