#include "Application/Window.h"
#include "Misc/PlatformMisc.h"
#include "Core/SpdLogging.h"

Window::Window(uint32_t Width, uint32_t Height, const char* Title)
	: m_Width(std::max<uint32_t>(Width, MINIMAL_WINDOW_SIZE))
	, m_Height(std::max<uint32_t>(Height, MINIMAL_WINDOW_SIZE))
	, m_Mode(EWindowMode::Windowed)
	, m_Handle(nullptr)
	, m_Title(Title)
{
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

	::RECT Rect
	{ 
		0l, 
		0l, 
		static_cast<long>(m_Width), 
		static_cast<long>(m_Height) 
	};
	VERIFY_WITH_SYSTEM_MESSAGE(::AdjustWindowRect(&Rect, WS_OVERLAPPEDWINDOW, false) != 0);

	HINSTANCE HInstance = reinterpret_cast<HINSTANCE>(PlatformMisc::GetApplicationHandle());
	VERIFY_WITH_SYSTEM_MESSAGE(HInstance);

	uint32_t ExtraWindowStyle = 0u;
	::HWND Handle = ::CreateWindowExW(
		0,
		WINDOW_CLASS_NAME,
		Settings.Title.to_wide().c_str(),
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

void Window::UpdateSize(uint32_t Width, uint32_t Height)
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

void Window::SetMode(EWindowMode Mode)
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

		UpdateSize(0u, 0u);
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

		UpdateSize(TargetClientWidth, TargetClientHeight);
	}
}

#endif // PLATFORM_WINDOWS
