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
	None,
	Down,
	Up,
	DoubleClick
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
	bool IsMoving = false;
	int16_t WheelDelta = 0;
	EKeyState State = EKeyState::Up;
	Math::Vector2 Position;
};

struct KeyboardEvent
{
	EKeyboardKey Key = EKeyboardKey::None;
	EKeyState State = EKeyState::Up;
	char InputChar = 0;
	InputModifiers Modifiers;
};

enum class EWindowStatus : uint8_t
{
	Activate,
	Inactivate,
	Destroyed,
};

enum class EMessageMasks
{
	None,
	Mouse = 1 << 0,
	Keyboard = 1 << 1,
	WindowResized = 1 << 2,
	WindowStatusChanged = 1 << 3,
	All = Mouse | Keyboard | WindowResized | WindowStatusChanged
};

class MessageHandler
{
public:
	virtual ~MessageHandler() = default;

	virtual void OnMouseEvent(const MouseEvent&) {}
	virtual void OnKeyboardEvent(const KeyboardEvent&) {}
	virtual void OnWindowResized(uint32_t, uint32_t) {}
	virtual void OnWindowStatusChanged(EWindowStatus) {}
};

class MessageRouter : public Singleton<MessageRouter>
{
public:
	void RegisterMessageHandler(MessageHandler* Handler);
	void DeregisterMessageHandler(MessageHandler* Handler);

	inline void DispatchMouseEvent(const MouseEvent& Event)
	{
		std::for_each(m_Handlers.begin(), m_Handlers.end(), [Event](MessageHandler* Handler) {
			if (Handler)
			{
				Handler->OnMouseEvent(Event);
			}
		});
	}

	inline void DispatchKeyboardEvent(const KeyboardEvent& Event)
	{
		std::for_each(m_Handlers.begin(), m_Handlers.end(), [Event](MessageHandler* Handler) {
			if (Handler)
			{
				Handler->OnKeyboardEvent(Event);
			}
		});
	}

	inline void DispatchWindowResized(uint32_t Width, uint32_t Height)
	{
		std::for_each(m_Handlers.begin(), m_Handlers.end(), [Width, Height](MessageHandler* Handler) {
			if (Handler)
			{
				Handler->OnWindowResized(Width, Height);
			}
		});
	}

	inline void DispatchWindowStatusChanged(EWindowStatus Status)
	{
		std::for_each(m_Handlers.begin(), m_Handlers.end(), [Status](MessageHandler* Handler) {
			if (Handler)
			{
				Handler->OnWindowStatusChanged(Status);
			}
		});
	}
private:
	std::vector<MessageHandler*> m_Handlers;
};
