#pragma once

#include "Core/Math/Math.h"
#include "Core/Singleton.h"

enum class EMouseButton : uint8_t
{
	Left,
	Right,
	Middle,
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

struct KeyEvent
{
	EKeyboardKey Key = EKeyboardKey::None;
	uint64_t RawKeyCode = 0;
};

class MessageHandler
{
public:
	virtual bool OnKeyUp(const KeyEvent&, const KeyModifiers&) { return false; }
	virtual bool OnKeyDown(const KeyEvent&, const KeyModifiers&) { return false; }
	virtual bool OnKeyChar(const char) { return false; }

	virtual bool OnMouseButtonUp(const EMouseButton) { return false; }
	virtual bool OnMouseButtonDown(const EMouseButton, const Math::Vector2&) { return false; }
	virtual bool OnMouseButtonDoubleClick(const EMouseButton, const Math::Vector2&) { return false; }
	virtual bool OnMouseMove(const Math::Vector2&) { return false; }
	virtual bool OnMouseWheel(const float, const Math::Vector2&) { return false; }

	virtual bool OnWindowResized(uint32_t, uint32_t) { return false; }
	virtual bool OnAppActive() { return false; }
	virtual bool OnAppInactive() { return false; }
	virtual bool OnAppQuit() { return false; }
};