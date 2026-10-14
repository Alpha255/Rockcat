#include "Misc/PlatformMisc.h"
#include "Application/Window.h"
#include "Application/Win32/Resource.h"
#include "Core/SpdLogging.h"

#if PLATFORM_WIN32

#include <Windows.h>
#include <windowsx.h>
#include <shlobj.h>

namespace
{
	/// Density of a 100% display. Every layout below is authored in logical units at that density.
	static constexpr UINT kBaseDpi = 96u;

	/// Values of the Win32 DPI enums, spelled out so this compiles against any SDK version.
	/// Some of them are only exposed by recent headers and this engine targets older ones too.
	static constexpr intptr_t kAwarenessPerMonitorV2 = -4;  /// DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2
	static constexpr int kAwarenessPerMonitorv1 = 2;        /// PROCESS_PER_MONITOR_DPI_AWARE

	/// Ask Windows for per monitor DPI scaling before the first window exists.
	///
	/// Without this the shell reports 96 DPI for every monitor and afterwards bitmap stretches the
	/// finished frame, which is the blurry, soft looking UI we had. Windows locks the awareness in
	/// the first time anything queries it, hence doing this before a single HWND is created.
	///
	/// The APIs are resolved dynamically: they live in user32.dll but only from a certain OS
	/// version on, and there is no point refusing to start on an old one.
	void EnablePerMonitorDpiAwareness()
	{
		if (const ::HMODULE User32 = ::GetModuleHandleW(L"user32.dll"))
		{
			/// Windows 10 1607+: additionally scales the non-client area itself and sends
			/// WM_DPICHANGED when the window moves between monitors of another density.
			if (const auto SetDpiAwarenessContext = reinterpret_cast<BOOL(WINAPI*)(intptr_t)>(
				::GetProcAddress(User32, "SetProcessDpiAwarenessContext")))
			{
				if (SetDpiAwarenessContext(kAwarenessPerMonitorV2))
				{
					return;
				}
			}

			/// Windows 8.1: per monitor scaling of the client area only.
			if (const auto SetDpiAwareness = reinterpret_cast<HRESULT(WINAPI*)(int)>(
				::GetProcAddress(User32, "SetProcessDpiAwareness")))
			{
				if (SUCCEEDED(SetDpiAwareness(kAwarenessPerMonitorv1)))
				{
					return;
				}
			}
		}

		/// Windows Vista fallback: not per monitor, but at least no stretch by the shell.
		::SetProcessDPIAware();
	}

	float QueryDpiScale(::HWND Handle)
	{
		if (!Handle)
		{
			return 1.0f;
		}

		if (const ::HMODULE User32 = ::GetModuleHandleW(L"user32.dll"))
		{
			if (const auto GetDpiForWindowFn = reinterpret_cast<UINT(WINAPI*)(::HWND)>(
				::GetProcAddress(User32, "GetDpiForWindow")))
			{
				return static_cast<float>(GetDpiForWindowFn(Handle)) / static_cast<float>(kBaseDpi);
			}

			if (const auto GetDpiForMonitorFn = reinterpret_cast<HRESULT(WINAPI*)(::HMONITOR, int, UINT*, UINT*)>(
				::GetProcAddress(User32, "GetDpiForMonitor")))
			{
				const ::HMONITOR Monitor = ::MonitorFromWindow(Handle, MONITOR_DEFAULTTONEAREST);

				UINT DpiX = kBaseDpi;
				UINT DpiY = kBaseDpi;
				if (SUCCEEDED(GetDpiForMonitorFn(Monitor, 0 /* MDT_EFFECTIVE_DPI */, &DpiX, &DpiY)))
				{
					return static_cast<float>(DpiX) / static_cast<float>(kBaseDpi);
				}
			}
		}

		return 1.0f;
	}
}

void InitializeDpiAwareness()
{
	EnablePerMonitorDpiAwareness();
}

float Window::GetDpiScale() const
{
	return QueryDpiScale(reinterpret_cast<::HWND>(m_Handle));
}

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

#endif // PLATFORM_WIN32
