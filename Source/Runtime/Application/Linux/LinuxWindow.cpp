#include "Application/Application.h"
#include "Application/Window.h"
#include "Core/SpdLogging.h"

#if PLATFORM_LINUX

#include <xcb/xcb.h>

#include <cstdlib>
#include <limits>
#include <memory>
#include <string_view>
#include <vector>

namespace
{
	/// Density of a 100% display, the one every UI size in the editor is authored at.
	static constexpr float kBaseDpi = 96.0f;

	/// The resource database and the EDID sizes below are whatever somebody configured or whatever
	/// the monitor claims, so anything below this is treated as bogus rather than as a real density.
	static constexpr float kMinimumDpi = 48.0f;

	/// _NET_WM_STATE actions, see the EWMH specification. They travel as data32[0] of the client
	/// message, hence they are not atoms.
	static constexpr uint32_t kNetWmStateRemove = 0u;
	static constexpr uint32_t kNetWmStateAdd = 1u;

	/// ICCCM size hint flags. Only these two are wanted, and pulling in Xlib.h for Xutil.h is not
	/// worth it for them.
	static constexpr uint32_t kSizeHintPMinSize = 1u << 4u;
	static constexpr uint32_t kSizeHintPBaseSize = 1u << 8u;

	/// The size hints property is a fixed array of longs: flags, x, y, width, height, min/max size,
	/// increments, aspect ratios, base size and gravity. Everything except the flags and the two
	/// sizes below is left at zero, which means "unset" to the window manager.
	static constexpr uint32_t kSizeHintValues = 18u;

	/// GetHandle() hands the address out and Window has no destructor of its own yet, so ownership
	/// stays here for the lifetime of the process. Closing the connection tears everything down
	/// with it anyway.
	std::vector<std::unique_ptr<LinuxWindowHandle>>& GetHandleRegistry()
	{
		static std::vector<std::unique_ptr<LinuxWindowHandle>> Registry;
		return Registry;
	}

	/// Every atom one operation needs in one place: interning one costs a round trip to the server,
	/// and none of them change over the life of a session.
	struct XcbAtoms
	{
		xcb_atom_t Protocols;
		xcb_atom_t DeleteWindow;
		xcb_atom_t State;
		xcb_atom_t StateFullscreen;
		xcb_atom_t NetWmName;
		xcb_atom_t Utf8String;
		xcb_atom_t NormalHints;
		xcb_atom_t SizeHints;

		explicit XcbAtoms(xcb_connection_t* Connection)
		{
			assert(Connection);

			Protocols = InternAtom(Connection, "WM_PROTOCOLS");
			DeleteWindow = InternAtom(Connection, "WM_DELETE_WINDOW");
			State = InternAtom(Connection, "_NET_WM_STATE");
			StateFullscreen = InternAtom(Connection, "_NET_WM_STATE_FULLSCREEN");
			NetWmName = InternAtom(Connection, "_NET_WM_NAME");
			Utf8String = InternAtom(Connection, "UTF8_STRING");
			NormalHints = InternAtom(Connection, "WM_NORMAL_HINTS");
			SizeHints = InternAtom(Connection, "WM_SIZE_HINTS");
		}

	private:
		static xcb_atom_t InternAtom(xcb_connection_t* Connection, std::string_view Name)
		{
			const xcb_intern_atom_cookie_t Cookie = ::xcb_intern_atom(Connection, 0u,
				static_cast<uint16_t>(Name.size()), Name.data());

			xcb_generic_error_t* Error = nullptr;
			xcb_intern_atom_reply_t* Reply = ::xcb_intern_atom_reply(Connection, Cookie, &Error);
			::free(Error);

			const xcb_atom_t Atom = Reply ? Reply->atom : XCB_NONE;
			::free(Reply);

			return Atom;
		}
	};

	/// The screen xcb_connect() picked for us. It is the one every size below is expressed against.
	xcb_screen_t* QueryScreen(xcb_connection_t* Connection, int PreferredScreenIndex)
	{
		assert(Connection);

		xcb_screen_iterator_t Iterator = ::xcb_setup_roots_iterator(::xcb_get_setup(Connection));
		for (int Index = 0; Index < PreferredScreenIndex && Iterator.rem > 0; ++Index)
		{
			::xcb_screen_next(&Iterator);
		}

		VERIFY(Iterator.data);
		return Iterator.data;
	}

	/// Highest number of longs ever read back out of a property here: the resource database of a
	/// desktop is a few kilobytes at most, while asking for "everything" would ask XCB for a reply
	/// larger than the maximum request size allows.
	static constexpr uint32_t kMaximumPropertyLength = 1024u * 1024u;

	/// Asks the X resource database of the display for Xft.dpi, which is where every toolkit on
	/// X11 reads the density from - and where Qt, GTK and the XSETTINGS daemon agree to write it.
	float QueryResourceDpi(xcb_connection_t* Connection, xcb_window_t Root)
	{
		constexpr std::string_view kResourceManagerName("RESOURCE_MANAGER");

		const xcb_intern_atom_cookie_t AtomCookie = ::xcb_intern_atom(Connection, 0u,
			static_cast<uint16_t>(kResourceManagerName.size()), kResourceManagerName.data());
		xcb_generic_error_t* AtomError = nullptr;
		xcb_intern_atom_reply_t* AtomReply = ::xcb_intern_atom_reply(Connection, AtomCookie, &AtomError);
		::free(AtomError);

		const xcb_atom_t ResourceManager = AtomReply ? AtomReply->atom : XCB_NONE;
		::free(AtomReply);

		if (ResourceManager == XCB_NONE)
		{
			return 0.0f;
		}

		/// One resource per line: "Xft.dpi:\t144", possibly qualified with wildcards and missing
		/// altogether on a desktop nobody ever configures.
		const xcb_get_property_cookie_t Cookie = ::xcb_get_property(Connection, 0u, Root, ResourceManager,
			XCB_ATOM_STRING, 0u, kMaximumPropertyLength);

		xcb_generic_error_t* Error = nullptr;
		xcb_get_property_reply_t* Reply = ::xcb_get_property_reply(Connection, Cookie, &Error);
		::free(Error);

		if (!Reply)
		{
			return 0.0f;
		}

		const std::string_view Resources(
			static_cast<const char*>(::xcb_get_property_value(Reply)),
			static_cast<size_t>(::xcb_get_property_value_length(Reply)));

		float Dpi = 0.0f;
		size_t LineBegin = 0u;
		while (LineBegin < Resources.size())
		{
			size_t LineEnd = Resources.find('\n', LineBegin);
			if (LineEnd == std::string_view::npos)
			{
				LineEnd = Resources.size();
			}

			const std::string_view Line = Resources.substr(LineBegin, LineEnd - LineBegin);
			LineBegin = LineEnd + 1u;

			/// Resource names may be bound loosely ("*.Xft.dpi", ".window.Xft.dpi" and such) and
			/// may be indented, none of which changes the name being looked for.
			const size_t NameBegin = Line.find("Xft.dpi");
			if (NameBegin == std::string_view::npos)
			{
				continue;
			}

			const size_t Separator = Line.find(':', NameBegin);
			if (Separator == std::string_view::npos)
			{
				continue;
			}

			std::string Value(Line.substr(Separator + 1u));
			const float Candidate = std::strtof(Value.c_str(), nullptr);
			if (Candidate >= kMinimumDpi)
			{
				Dpi = Candidate;
				break;
			}
		}

		::free(Reply);
		return Dpi;
	}

	/// Resizing through the window itself rather than the window manager: XCB only forwards the
	/// request, so asking for the size again afterwards (UpdateSize(0, 0)) is what tells us what
	/// was actually granted.
	void ConfigureSize(const LinuxWindowHandle& Handle, uint32_t Width, uint32_t Height)
	{
		assert(Handle.Connection && Handle.Window != XCB_NONE);

		const uint32_t Values[]
		{
			Width,
			Height
		};
		::xcb_configure_window(Handle.Connection, Handle.Window,
			XCB_CONFIG_WINDOW_WIDTH | XCB_CONFIG_WINDOW_HEIGHT, Values);
		::xcb_flush(Handle.Connection);
	}

	/// Adds or removes a _NET_WM_STATE property of the window. The window manager is the one that
	/// owns the state, so it is asked through a client message to the root instead of being told.
	void SendNetWmState(const LinuxWindowHandle& Handle, const XcbAtoms& Atoms, uint32_t Action, xcb_atom_t Property)
	{
		assert(Handle.Connection && Handle.Screen);

		xcb_client_message_event_t Event{};
		Event.response_type = XCB_CLIENT_MESSAGE;
		Event.format = 32u;
		Event.window = Handle.Window;
		Event.type = Atoms.State;
		Event.data.data32[0u] = Action;
		Event.data.data32[1u] = Property;
		Event.data.data32[2u] = 0u;
		Event.data.data32[3u] = 0u;
		Event.data.data32[4u] = 0u;

		::xcb_send_event(Handle.Connection, 0u, Handle.Screen->root,
			XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY | XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT,
			reinterpret_cast<const char*>(&Event));
		::xcb_flush(Handle.Connection);
	}
}

/// Nothing to set up: X11 has no process wide awareness to opt into before the first window, the
/// server hands the real pixel size to anybody that asks. Kept because each platform decides that
/// for itself.
void InitializeDpiAwareness()
{
}

float Window::GetDpiScale() const
{
	const auto* Handle = static_cast<const LinuxWindowHandle*>(m_Handle);
	if (!Handle || !Handle->Connection || !Handle->Screen)
	{
		return 1.0f;
	}

	/// What every toolkit on X11 reads: Xft.dpi out of the resource database.
	const float ResourceDpi = QueryResourceDpi(Handle->Connection, Handle->Screen->root);
	if (ResourceDpi >= kMinimumDpi)
	{
		return ResourceDpi / kBaseDpi;
	}

	/// Otherwise fall back to what the screen physically is: the EDID sizes the server reports are
	/// often wrong, which is why the resource above wins, but they are better than guessing 1.0.
	const xcb_screen_t& Screen = *Handle->Screen;
	if (Screen.width_in_millimeters > 0u)
	{
		const float PhysicalDpi = static_cast<float>(Screen.width_in_pixels) * 25.4f
			/ static_cast<float>(Screen.width_in_millimeters);

		if (PhysicalDpi >= kMinimumDpi)
		{
			return PhysicalDpi / kBaseDpi;
		}
	}

	return 1.0f;
}

Window::Window(const WindowSettings& Settings)
	: m_Width(std::max<uint32_t>(Settings.Width, MINIMAL_WINDOW_SIZE))
	, m_Height(std::max<uint32_t>(Settings.Height, MINIMAL_WINDOW_SIZE))
	, m_Mode(EWindowMode::Windowed)
	, m_Handle(nullptr)
{
	int ScreenIndex = 0;
	xcb_connection_t* Connection = ::xcb_connect(nullptr, &ScreenIndex);
	VERIFY_WITH_SYSTEM_MESSAGE(Connection);
	VERIFY_WITH_SYSTEM_MESSAGE(::xcb_connection_has_error(Connection) == 0);

	auto& HandleInstance = GetHandleRegistry().emplace_back(std::make_unique<LinuxWindowHandle>());
	LinuxWindowHandle& XcbWindow = *HandleInstance;
	XcbWindow.Connection = Connection;
	XcbWindow.Screen = QueryScreen(Connection, ScreenIndex);

	const xcb_screen_t& Screen = *XcbWindow.Screen;
	XcbWindow.Window = ::xcb_generate_id(Connection);

	/// Centered on whichever screen the connection landed on. The window may well ask for more than
	/// that screen has to give, and a negative margin would wrap around into an unsigned position.
	const uint32_t PositionX = Screen.width_in_pixels > m_Width ? (Screen.width_in_pixels - m_Width) / 2u : 0u;
	const uint32_t PositionY = Screen.height_in_pixels > m_Height ? (Screen.height_in_pixels - m_Height) / 2u : 0u;

	/// The event mask decides what LinuxApplication gets to see later on: key and button press and
	/// release, the pointer moving, the client area changing shape (ConfigureNotify comes out of
	/// STRUCTURE_NOTIFY), the window gaining or losing focus, and EXPOSURE because some window
	/// managers still expect the application to redraw on it.
	const uint32_t EventMask =
		XCB_EVENT_MASK_KEY_PRESS |
		XCB_EVENT_MASK_KEY_RELEASE |
		XCB_EVENT_MASK_BUTTON_PRESS |
		XCB_EVENT_MASK_BUTTON_RELEASE |
		XCB_EVENT_MASK_POINTER_MOTION |
		XCB_EVENT_MASK_BUTTON_MOTION |
		XCB_EVENT_MASK_ENTER_WINDOW |
		XCB_EVENT_MASK_LEAVE_WINDOW |
		XCB_EVENT_MASK_FOCUS_CHANGE |
		XCB_EVENT_MASK_PROPERTY_CHANGE |
		XCB_EVENT_MASK_STRUCTURE_NOTIFY |
		XCB_EVENT_MASK_EXPOSURE;

	const uint32_t Values[]
	{
		Screen.black_pixel,
		EventMask
	};
	::xcb_create_window(Connection, XCB_COPY_FROM_PARENT, XcbWindow.Window, Screen.root,
		static_cast<int16_t>(PositionX), static_cast<int16_t>(PositionY),
		static_cast<uint16_t>(std::min<uint32_t>(m_Width, std::numeric_limits<uint16_t>::max())),
		static_cast<uint16_t>(std::min<uint32_t>(m_Height, std::numeric_limits<uint16_t>::max())),
		0u, XCB_WINDOW_CLASS_INPUT_OUTPUT, Screen.root_visual,
		XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK, Values);

	const XcbAtoms Atoms(Connection);

	/// Both the old WM_NAME (STRING) and the EWMH _NET_WM_NAME (UTF-8): modern window managers only
	/// read the latter, older ones only the former.
	const std::string_view Title(Settings.Title);
	::xcb_change_property(Connection, XCB_PROP_MODE_REPLACE, XcbWindow.Window,
		XCB_ATOM_WM_NAME, XCB_ATOM_STRING, 8u, static_cast<uint32_t>(Title.size()), Title.data());
	::xcb_change_property(Connection, XCB_PROP_MODE_REPLACE, XcbWindow.Window,
		Atoms.NetWmName, Atoms.Utf8String, 8u, static_cast<uint32_t>(Title.size()), Title.data());
	::xcb_change_property(Connection, XCB_PROP_MODE_REPLACE, XcbWindow.Window,
		XCB_ATOM_WM_ICON_NAME, XCB_ATOM_STRING, 8u, static_cast<uint32_t>(Title.size()), Title.data());

	/// Without WM_PROTOCOLS carrying WM_DELETE_WINDOW the X server kills the connection outright
	/// when the close button is pressed, and neither the application nor the viewports it owns get
	/// to shut anything down.
	::xcb_change_property(Connection, XCB_PROP_MODE_REPLACE, XcbWindow.Window,
		Atoms.Protocols, XCB_ATOM_ATOM, 32u, 1u, &Atoms.DeleteWindow);

	/// The equivalent of answering WM_GETMINMAXINFO: nothing may scale the window below the point
	/// where a swap chain still makes sense. This is the ICCCM WM_SIZE_HINTS array, 18 longs in the
	/// order of the structure: flags, position, size, minimum, maximum, increments, then the two
	/// aspect ratios (two values each), and finally the base size and the gravity.
	uint32_t SizeHints[kSizeHintValues] {};
	SizeHints[0u] = kSizeHintPMinSize | kSizeHintPBaseSize;
	SizeHints[5u] = MINIMAL_WINDOW_SIZE;   /// min_width
	SizeHints[6u] = MINIMAL_WINDOW_SIZE;   /// min_height
	SizeHints[15u] = m_Width;              /// base_width
	SizeHints[16u] = m_Height;             /// base_height
	::xcb_change_property(Connection, XCB_PROP_MODE_REPLACE, XcbWindow.Window,
		Atoms.NormalHints, Atoms.SizeHints, 32u, kSizeHintValues, SizeHints);

	::xcb_map_window(Connection, XcbWindow.Window);
	::xcb_flush(Connection);

	m_Handle = static_cast<const void*>(HandleInstance.get());

	SetMode(Settings.Mode);
}

void Window::UpdateSize(uint32_t Width, uint32_t Height)
{
	const auto* Handle = static_cast<const LinuxWindowHandle*>(m_Handle);

	if ((Width == 0u || Height == 0u) && Handle && Handle->Connection && Handle->Window != XCB_NONE)
	{
		/// Asked for the real thing instead of trusting what was configured: the window manager is
		/// free to hand out something else, and the one size everything else follows has to be the
		/// size the server ended up with.
		const xcb_get_geometry_cookie_t Cookie = ::xcb_get_geometry(Handle->Connection, Handle->Window);

		xcb_generic_error_t* Error = nullptr;
		if (xcb_get_geometry_reply_t* Reply = ::xcb_get_geometry_reply(Handle->Connection, Cookie, &Error))
		{
			Width = Reply->width;
			Height = Reply->height;
			::free(Reply);
		}
		::free(Error);
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

	auto* Handle = static_cast<LinuxWindowHandle*>(m_Handle);
	VERIFY(Handle && Handle->Connection && Handle->Screen);

	const EWindowMode PreviousMode = m_Mode;
	m_Mode = Mode;

	LinuxWindowHandle& XcbWindow = *Handle;
	const XcbAtoms Atoms(XcbWindow.Connection);

	if (Mode == EWindowMode::Windowed)
	{
		SendNetWmState(XcbWindow, Atoms, kNetWmStateRemove, Atoms.StateFullscreen);

		/// Nothing remembers this size except this window: going back has to ask for it again.
		if (XcbWindow.WindowedWidth != 0u && XcbWindow.WindowedHeight != 0u)
		{
			ConfigureSize(XcbWindow, XcbWindow.WindowedWidth, XcbWindow.WindowedHeight);
		}

		UpdateSize(0u, 0u);
	}
	else if (Mode == EWindowMode::BorderlessFullscreen || Mode == EWindowMode::ExclusiveFullscreen)
	{
		if (PreviousMode == EWindowMode::Windowed)
		{
			UpdateSize(0u, 0u);

			XcbWindow.WindowedWidth = m_Width;
			XcbWindow.WindowedHeight = m_Height;
		}

		/// Both kinds end up as a window without decorations covering the whole screen: owning a
		/// display mode would go through XRandR, which nothing here has a dependency on yet - and
		/// on modern compositors it only ever gets in the way.
		SendNetWmState(XcbWindow, Atoms, kNetWmStateAdd, Atoms.StateFullscreen);

		const uint32_t ScreenWidth = XcbWindow.Screen->width_in_pixels;
		const uint32_t ScreenHeight = XcbWindow.Screen->height_in_pixels;

		ConfigureSize(XcbWindow, ScreenWidth, ScreenHeight);
		UpdateSize(ScreenWidth, ScreenHeight);
	}
}

#endif // PLATFORM_LINUX
