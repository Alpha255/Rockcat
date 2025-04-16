#include "Core/Window.h"
#include "Core/System.h"
#include "Engine/Application/Win32/Resource.h"
#include "Engine/Application/BaseApplication.h"
#include "Engine/Services/SpdLogService.h"

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

	if (auto App = reinterpret_cast<BaseApplication*>(::GetWindowLongPtrA(HWnd, 0)))
	{
		App->ProcessMessage(Message, WParam, LParam);
	}

	return ::DefWindowProcA(HWnd, Message, WParam, LParam);
}

Window::Window(const WindowCreateInfo& CreateInfo, BaseApplication* Owner)
	: m_MinWidth(std::max<uint32_t>(CreateInfo.MinWidth, MINIMAL_WINDOW_SIZE))
	, m_MinHeight(std::max<uint32_t>(CreateInfo.MinHeight, MINIMAL_WINDOW_SIZE))
	, m_Width(std::max<uint32_t>(CreateInfo.Width, CreateInfo.MinWidth))
	, m_Height(std::max<uint32_t>(CreateInfo.Height, CreateInfo.MinHeight))
{
	::HINSTANCE HInstance = reinterpret_cast<::HINSTANCE>(System::GetCurrentModuleHandle());
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
		static_cast<void*>(Owner));
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

