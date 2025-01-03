#pragma once

#include "Core/Math/Math.h"

enum class EMouseButton : uint8_t
{
	LButton,
	RButton,
	MButton,
	None = 0xFF,
};

enum class EKeyboardKey : uint8_t
{
	Tab,
	Enter,
	Shift,
	Ctrl,
	Escape,
	Alt,
	Space = ' ',
	PageUp,
	PageDown,
	Home,
	Left,
	Up,
	Right,
	Down,	
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
	F1,
	Other,
	None = 0xFF
};

enum class EKeyState : uint8_t
{
	Down,
	Up,
	DBClick
};

struct InputModifiers
{
	bool Ctrl = false;
	bool Shift = false;
	bool Alt = false;
};

struct MouseEvent
{
	EMouseButton Button;
	bool OnMove = false;
	int16_t WheelDelta = 0;
	EKeyState State = EKeyState::Up;
	Math::Vector2 Position;
	InputModifiers Modifiers;
};

struct KeyboardEvent
{
	EKeyboardKey Key = EKeyboardKey::None;
	EKeyState State = EKeyState::Up;
	char InputChar = 0;
	InputModifiers Modifiers;
};

class IInputHandler
{
public:
	virtual ~IInputHandler() = default;

	virtual void OnMouseEvent(const MouseEvent&) {}
	virtual void OnKeyboardEvent(const KeyboardEvent&) {}
	virtual void OnWindowResized(uint32_t, uint32_t) {}
};

