#include "Core/Definitions.h"

#if PLATFORM_WINDOWS

#include "Application/Windows/WindowsWindow.h"
#include "Core/SpdLogging.h"

const std::wstring_view WindowsWindow::WindowClassName(L"RockCatWindow");

WindowsWindow::WindowsWindow(HINSTANCE HInstance, uint32_t Width, uint32_t Height, const char* Title)
	: Window(Width, Height, Title)
{
	assert(HInstance);

	::RECT Rect
	{ 
		0l, 
		0l, 
		static_cast<long>(m_Width), 
		static_cast<long>(m_Height) 
	};
	VERIFY_WITH_SYSTEM_MESSAGE(::AdjustWindowRect(&Rect, WS_OVERLAPPEDWINDOW, false) != 0);

	std::wstring WindowTitle = string(Title).to_wide();

	uint32_t ExtraWindowStyle = 0u;
	::HWND Handle = ::CreateWindowExW(
		0,
		WindowClassName.data(),
		WindowTitle.c_str(),
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

	SetMode(m_Mode);
}

void WindowsWindow::SetTitle(const char* Title)
{
	std::wstring WindowTile = string(Title).to_wide();
	if (!WindowTile.empty())
	{
		VERIFY_WITH_SYSTEM_MESSAGE(::SetWindowTextW(reinterpret_cast<::HWND>(m_Handle), WindowTile.c_str()));
	}
}

void WindowsWindow::OnResize(uint32_t Width, uint32_t Height)
{
	if (Width == 0 || Height == 0)
	{
		::RECT Rect{};
		VERIFY_WITH_SYSTEM_MESSAGE(::GetClientRect(reinterpret_cast<::HWND>(m_Handle), &Rect) != 0);
		Width = Rect.right - Rect.left;
		Height = Rect.bottom - Rect.top;
	}

	if (m_Width != Width || m_Height != Height)
	{
		LOG_DEBUG(LogDefault, "Set window mode to \"{}\", Window resized from @{}x{} to @{}x{}", magic_enum::enum_name(m_Mode), m_Width, m_Height, Width, Height);
		m_Width = Width;
		m_Height = Height;
	}
}

void WindowsWindow::SetMode(EWindowMode Mode)
{
	if (m_Mode == Mode)
	{
		return;
	}

	static ::WINDOWPLACEMENT s_PreFullscreenWindowPlacement{};

	EWindowMode PreviousMode = m_Mode;
	m_Mode = Mode;

	::HWND hWnd = reinterpret_cast<::HWND>(m_Handle);
	const long WindowedStyle = WS_OVERLAPPEDWINDOW;
	const long FullscreenStyle = WS_POPUP;
	long WindowStyle = ::GetWindowLong(hWnd, GWL_STYLE);

	if (Mode == EWindowMode::Windowed)
	{
		WindowStyle &= ~FullscreenStyle;
		WindowStyle |= WindowedStyle;

		::SetWindowLong(hWnd, GWL_STYLE, WindowStyle);
		::SetWindowPos(hWnd, nullptr, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);

		if (s_PreFullscreenWindowPlacement.length)
		{
			::SetWindowPlacement(hWnd, &s_PreFullscreenWindowPlacement);
		}

		if (HICON hIcon = (HICON)::GetClassLongPtr(hWnd, GCLP_HICON))
		{
			::SendMessageW(hWnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
		}

		OnResize(0u, 0u);
	}
	else if (Mode == EWindowMode::BorderlessFullscreen || Mode == EWindowMode::ExclusiveFullscreen)
	{
		if (PreviousMode == EWindowMode::Windowed)
		{
			s_PreFullscreenWindowPlacement.length = sizeof(::WINDOWPLACEMENT);
			::GetWindowPlacement(hWnd, &s_PreFullscreenWindowPlacement);
		}

		WindowStyle &= ~WindowedStyle;
		WindowStyle |= FullscreenStyle;

		::SetWindowLong(hWnd, GWL_STYLE, WindowStyle);
		::SetWindowPos(hWnd, nullptr, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);

		if (Mode == EWindowMode::BorderlessFullscreen)
		{
			::ShowWindow(hWnd, SW_RESTORE);
		}

		::RECT ClientRect{};
		::GetClientRect(hWnd, &ClientRect);

		::HMONITOR Monitor = ::MonitorFromWindow(hWnd, Mode == EWindowMode::ExclusiveFullscreen ? MONITOR_DEFAULTTOPRIMARY : MONITOR_DEFAULTTONEAREST);
		::MONITORINFO MonitorInfo{ sizeof(::MONITORINFO) };
		::GetMonitorInfo(Monitor, &MonitorInfo);

		long MonitorWidth = MonitorInfo.rcMonitor.right - MonitorInfo.rcMonitor.left;
		long TargetClientWidth = Mode == EWindowMode::ExclusiveFullscreen ?
			std::min<long>(MonitorWidth, ClientRect.right - ClientRect.left) :
			MonitorWidth;

		long MonitorHeight = MonitorInfo.rcMonitor.bottom - MonitorInfo.rcMonitor.top;
		long TargetClientHeight = Mode == EWindowMode::ExclusiveFullscreen ?
			std::min<long>(MonitorHeight, ClientRect.bottom - ClientRect.top) :
			MonitorHeight;

		::SetWindowPos(hWnd, nullptr, MonitorInfo.rcMonitor.left, MonitorInfo.rcMonitor.top, TargetClientWidth, TargetClientHeight, 
			SWP_NOZORDER | SWP_NOACTIVATE | ((m_Mode == EWindowMode::ExclusiveFullscreen) ? SWP_NOSENDCHANGING : 0));

		OnResize(TargetClientWidth, TargetClientHeight);
	}
}

#endif // PLATFORM_WINDOWS
