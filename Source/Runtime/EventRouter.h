#pragma once

#include "Core/Math/Math.h"
#include "Core/Singleton.h"

enum class EMouseButton : uint8_t
{
	LButton,
	RButton,
	MButton,
	None = 0xFF,
};

enum class EKeyboardKey : uint8_t
{
	Esc,
	Tab,
	Enter,
	LeftShift,
	RightShift,
	LeftCtrl,
	RightCtrl,
	LeftAlt,
	RightAlt,
	CapsLock,
	Insert,
	Delete,
	Home,
	End,
	PageUp,
	PageDown,
	Left,
	Up,
	Right,
	Down,
	F1,
	F2,
	F3,
	F4,
	F5,
	F6,
	F7,
	F8,
	F9,
	F10,
	F11,
	F12,
	Space = ' ',
	LeftMenu,
	RightMenu,
	Backspace,
	Num0 = '0',
	Num1 = '1',
	Num2 = '2',
	Num3 = '3',
	Num4 = '4',
	Num5 = '5',
	Num6 = '6',
	Num7 = '7',
	Num8 = '8',
	Num9 = '9',
	A = 'A',
	B = 'B',
	C = 'C',
	D = 'D',
	E = 'E',
	F = 'F',
	G = 'G',
	H = 'H',
	I = 'I',
	J = 'J',
	K = 'K',
	L = 'L',
	M = 'M',
	N = 'N',
	O = 'O',
	P = 'P',
	Q = 'Q',
	R = 'R',
	S = 'S',
	T = 'T',
	U = 'U',
	V = 'V',
	W = 'W',
	X = 'X',
	Y = 'Y',
	Z = 'Z',
	OEM,
	None = 0xFF
};

enum class EKeyState : uint8_t
{
	None,
	Down,
	Up,
	DoubleClick
};

struct KeyModifiers
{
	bool LeftCtrl = false;
	bool LeftShift = false;
	bool LeftAlt = false;
	bool LeftMenu = false;
	bool RightCtrl = false;
	bool RightShift = false;
	bool RightAlt = false;
	bool RightMenu = false;
	bool CapsLock = false;
};

struct MouseButtonEvent
{
	EMouseButton Button;
	EKeyState KeyState = EKeyState::None;
};

using MouseMoveEvent = Math::Vector2;

struct MouseWheelEvent
{
	float Delta = 0;
};

struct KeyEvent
{
	EKeyboardKey Key = EKeyboardKey::None;
	int32_t RawKeyCode = 0;
	EKeyState KeyState = EKeyState::Up;
	KeyModifiers Modifiers;
};

struct CharEvent
{
	char Character = 0;
};

enum class EWindowStatus : uint8_t
{
	Active,
	Inactive,
	Destroyed,
};

enum class EEventMask : uint8_t
{
	None,
	WindowStatus = 1 << 0,
	WindowResize = 1 << 1,
	MouseButton = 1 << 2,
	MouseMove = 1 << 3,
	MouseWheel = 1 << 4,
	KeyEvent = 1 << 5,
	CharEvent = 1 << 6,
	WindowEvent = WindowStatus | WindowResize,
	MouseEvent = MouseButton | MouseMove | MouseWheel,
	KeyboardEvent = KeyEvent | CharEvent,
	All = WindowEvent | MouseEvent | KeyboardEvent,
};
ENUM_FLAG_OPERATORS(EEventMask)

class EventHandler
{
public:
	EventHandler(EEventMask Mask = EEventMask::All);

	virtual ~EventHandler() = default;

	virtual void OnMouseButtonEvent(const MouseButtonEvent&) {}
	virtual void OnMouseMoveEvent(const MouseMoveEvent&) {}
	virtual void OnMouseWheelEvent(const MouseWheelEvent&) {}
	virtual void OnKeyEvent(const KeyEvent&) {}
	virtual void OnCharEvent(const CharEvent&) {}
	virtual void OnWindowResized(uint32_t, uint32_t) {}
	virtual void OnWindowStatusChanged(EWindowStatus) {}

	inline void SetMessageMask(EEventMask Mask) { m_MessageMask = m_MessageMask | Mask; }
	inline void ResetMessageMask(EEventMask Mask) { m_MessageMask = m_MessageMask & ~Mask; }

	inline EEventMask GetMessageMask() const { return m_MessageMask; }
private:
	EEventMask m_MessageMask;
};

class EventRouter : public Singleton<EventRouter>
{
public:
	void RegisterEventHandler(EventHandler* Handler);
	void DeregisterEventHandler(EventHandler* Handler);

	inline void DispatchMouseButtonEvent(const MouseButtonEvent& Event)
	{
		std::for_each(m_Handlers.begin(), m_Handlers.end(), [Event](EventHandler* Handler) {
			if (Handler && EnumHasAllFlags(Handler->GetMessageMask(), EEventMask::MouseButton))
			{
				Handler->OnMouseButtonEvent(Event);
			}
		});
	}

	inline void DispatchMouseMoveEvent(const MouseMoveEvent& Event)
	{
		std::for_each(m_Handlers.begin(), m_Handlers.end(), [Event](EventHandler* Handler) {
			if (Handler && EnumHasAllFlags(Handler->GetMessageMask(), EEventMask::MouseMove))
			{
				Handler->OnMouseMoveEvent(Event);
			}
		});
	}

	inline void DispatchMouseWheelEvent(const MouseWheelEvent& Event)
	{
		std::for_each(m_Handlers.begin(), m_Handlers.end(), [Event](EventHandler* Handler) {
			if (Handler && EnumHasAllFlags(Handler->GetMessageMask(), EEventMask::MouseMove))
			{
				Handler->OnMouseWheelEvent(Event);
			}
		});
	}

	inline void DispatchKeyEvent(const KeyEvent& Event)
	{
		std::for_each(m_Handlers.begin(), m_Handlers.end(), [Event](EventHandler* Handler) {
			if (Handler && EnumHasAllFlags(Handler->GetMessageMask(), EEventMask::KeyEvent))
			{
				Handler->OnKeyEvent(Event);
			}
		});
	}

	inline void DispatchCharEvent(const CharEvent& Event)
	{
		std::for_each(m_Handlers.begin(), m_Handlers.end(), [Event](EventHandler* Handler) {
			if (Handler && EnumHasAllFlags(Handler->GetMessageMask(), EEventMask::CharEvent))
			{
				Handler->OnCharEvent(Event);
			}
		});
	}

	inline void DispatchWindowResized(uint32_t Width, uint32_t Height)
	{
		std::for_each(m_Handlers.begin(), m_Handlers.end(), [Width, Height](EventHandler* Handler) {
			if (Handler && EnumHasAllFlags(Handler->GetMessageMask(), EEventMask::WindowResize))
			{
				Handler->OnWindowResized(Width, Height);
			}
		});
	}

	inline void DispatchWindowStatusChanged(EWindowStatus Status)
	{
		std::for_each(m_Handlers.begin(), m_Handlers.end(), [Status](EventHandler* Handler) {
			if (Handler && EnumHasAllFlags(Handler->GetMessageMask(), EEventMask::WindowStatus))
			{
				Handler->OnWindowStatusChanged(Status);
			}
		});
	}
private:
	std::vector<EventHandler*> m_Handlers;
};

class MessageHandler
{
public:
	virtual bool OnKeyUp(const KeyEvent&) { return false; }
	virtual bool OnKeyDown(const KeyEvent&) { return false; }
	virtual bool OnKeyChar(const char) { return false; }

	virtual bool OnMouseButtonUp(const EMouseButton) { return false; }
	virtual bool OnMouseButtonDown(const EMouseButton, const Math::Vector2&) { return false; }
	virtual bool OnMouseButtonDoubleClick(const EMouseButton, const Math::Vector2&) { return false; }
	virtual bool OnMouseMove(const Math::Vector2&) { return false; }
	virtual bool OnMouseWheel(const float, const Math::Vector2&) { return false; }

	virtual bool OnWindowResized(uint32_t, uint32_t) { return false; }
	virtual bool OnWindowActive() { return false; }
	virtual bool OnWindowInactive() { return false; }
	virtual bool OnWindowDestroy() { return false; }
};