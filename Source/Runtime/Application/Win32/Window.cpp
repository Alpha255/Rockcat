#include "System/System.h"
#include "Application/Window.h"
#include "Application/Win32/Resource.h"
#include "Services/SpdLogService.h"

#if PLATFORM_WIN32

#include <Windows.h>
#include <windowsx.h>
#include <shlobj.h>

Window::Window(const WindowSettings& Settings)
	: m_Width(std::max<uint32_t>(Settings.Width, MINIMAL_WINDOW_SIZE))
	, m_Height(std::max<uint32_t>(Settings.Height, MINIMAL_WINDOW_SIZE))
	, m_Mode(EWindowMode::Windowed)
	, m_Handle(nullptr)
{
	::RECT Rect
	{ 
		0l, 
		0l, 
		static_cast<long>(m_Width), 
		static_cast<long>(m_Height) 
	};
	VERIFY_WITH_SYSTEM_MESSAGE(::AdjustWindowRect(&Rect, WS_OVERLAPPEDWINDOW, false) != 0);

	HINSTANCE HInstance = reinterpret_cast<HINSTANCE>(System::GetApplicationInstance());
	VERIFY_WITH_SYSTEM_MESSAGE(HInstance);

	uint32_t ExtraWindowStyle = 0u;
	::HWND Handle = ::CreateWindowExW(
		0,
		WINDOW_CLASS_NAME,
		StringUtils::ToWide(Settings.Title).c_str(),
		WS_OVERLAPPEDWINDOW ^ ExtraWindowStyle,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		Rect.right - Rect.left,
		Rect.bottom - Rect.top,
		nullptr,
		nullptr,
		HInstance,
		nullptr);
	VERIFY_WITH_SYSTEM_MESSAGE(Handle);

	::ShowWindow(Handle, SW_SHOWDEFAULT);
	VERIFY_WITH_SYSTEM_MESSAGE(::UpdateWindow(Handle) != 0);

	m_Handle = reinterpret_cast<void*>(Handle);

	SetMode(Settings.Mode);
}

void Window::UpdateSize()
{
	::RECT Rect;
	VERIFY_WITH_SYSTEM_MESSAGE(::GetClientRect(reinterpret_cast<::HWND>(m_Handle), &Rect) != 0);
	uint32_t Width = static_cast<uint32_t>(Rect.right - Rect.left);
	uint32_t Height = static_cast<uint32_t>(Rect.bottom - Rect.top);

	if (m_Width != Width || m_Height != Height)
	{
		LOG_DEBUG("System: Window resized, width = {}, height = {}", m_Width, m_Height);
		m_Width = Width;
		m_Height = Height;
	}
}

void Window::SetMode(EWindowMode Mode)
{
	if (m_Mode == Mode)
	{
		return;
	}

	m_Mode = Mode;

	::HWND hWnd = reinterpret_cast<::HWND>(m_Handle);
	const ::LONG WindowedStyle = WS_OVERLAPPEDWINDOW;
	const ::LONG FullscreenStyle = WS_POPUP;
	::LONG WindowStyle = ::GetWindowLong(hWnd, GWL_STYLE);
	::UINT Flags = SWP_NOMOVE | SWP_NOZORDER | SWP_FRAMECHANGED;
	
	::RECT WindowRect;
	::GetClientRect(hWnd, &WindowRect);

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

		::HMONITOR Monitor = ::MonitorFromWindow(hWnd, Mode == EWindowMode::ExclusiveFullscreen ? MONITOR_DEFAULTTOPRIMARY : MONITOR_DEFAULTTONEAREST);
		::MONITORINFO MonitorInfo;
		MonitorInfo.cbSize = sizeof(::MONITORINFO);
		::GetMonitorInfo(Monitor, &MonitorInfo);

		Width = static_cast<int32_t>(MonitorInfo.rcMonitor.right - MonitorInfo.rcMonitor.left);
		Height = static_cast<int32_t>(MonitorInfo.rcMonitor.bottom - MonitorInfo.rcMonitor.top);
	}

	::SetWindowLong(hWnd, GWL_STYLE, WindowStyle);
	::SetWindowPos(hWnd, nullptr, Left, Top, Width, Height, Flags);

	UpdateSize();
}

#endif // PLATFORM_WIN32
