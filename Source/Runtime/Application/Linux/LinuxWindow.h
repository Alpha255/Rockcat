#pragma once

#include "Core/Definitions.h"

#if PLATFORM_LINUX

#include <xcb/xcb.h>

/// What GetHandle() points at on Linux. Win32 can get away with the HWND alone because the window
/// server is asked through global APIs, but XCB hands everything over through an explicit
/// connection - and that one is per window, so both halves travel together in here.
///
/// Anyone that needs them (the XCB surface creation above all) downcasts the opaque handle:
///   auto* Handle = static_cast<const LinuxWindowHandle*>(Window.GetHandle());
struct LinuxWindowHandle
{
	xcb_connection_t* Connection = nullptr;
	xcb_window_t Window = XCB_NONE;

	/// The window was realized on this screen, and every size or mode below is expressed within it.
	/// Owned by the server - it points into the reply of xcb_get_setup().
	xcb_screen_t* Screen = nullptr;

	/// Size the window had before going full screen: taking the state off again does not put it
	/// back, that is left to whoever asked for full screen in the first place.
	uint32_t WindowedWidth = 0u;
	uint32_t WindowedHeight = 0u;
};

#endif // PLATFORM_LINUX
