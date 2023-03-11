#pragma once

#include "Core/Math/Math.h"

NAMESPACE_START(Gear)

enum class EMouseButton : uint8_t
{
	LButton,
	RButton,
	MButton,
	None = std::numeric_limits<uint8_t>().max(),
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
	None = std::numeric_limits<uint8_t>().max()
};

enum class EKeyState : uint8_t
{
	Down,
	Up,
	DBClick
};

struct InputModifiers
{
	bool8_t Ctrl = false;
	bool8_t Shift = false;
	bool8_t Alt = false;
};

struct MouseEvent
{
	EMouseButton Button;
	bool8_t OnMove = false;
	int8_t WheelDelta = 0;
	EKeyState State = EKeyState::Up;
	Math::Vector2 Position;
	InputModifiers Modifiers;
};

struct KeyboardEvent
{
	EKeyboardKey Key = EKeyboardKey::None;
	EKeyState State = EKeyState::Up;
	char8_t InputChar = 0;
	InputModifiers Modifiers;
};

class IInputHandler
{
public:
	virtual ~IInputHandler() = default;

	virtual void OnMouseEvent(const MouseEvent& Mouse) = 0;

	virtual void OnKeyboardEvent(const KeyboardEvent& Keyboard) = 0;

	virtual void OnWindowResized(uint32_t Width, uint32_t Height) = 0;
};

NAMESPACE_END(Gear)