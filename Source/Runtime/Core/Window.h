#pragma once

#include "Runtime/Core/InputState.h"

NAMESPACE_START(Gear)

struct WindowDesc
{
	uint32_t Width = 0u;
	uint32_t Height = 0u;
	uint32_t MinWidth = 0u;
	uint32_t MinHeight = 0u;
	const char8_t* Title = "MainWindow";
};

class Window
{
public:
	enum class EState : uint8_t
	{
		LostFocus,
		OnFocus,
		Destroyed,
	};

	Window(const WindowDesc& Desc, IInputHandler* InputHandler);

	inline const uint32_t Width() const
	{
		return m_Width;
	}

	inline const uint32_t Height() const
	{
		return m_Height;
	}

	inline const uint64_t Handle() const
	{
		return m_Handle;
	}

	inline const bool8_t Active() const
	{
		return m_State == EState::OnFocus;
	}

	inline const bool8_t Destroyed() const
	{
		return m_State == EState::Destroyed;
	}

	inline const EState State() const
	{
		return m_State;
	}

	void ProcessMessage(uint32_t Message, size_t WParam, intptr_t LParam);

	void PollMessage();
protected:
	void UpdateSize(bool8_t Signal = false);

	void OnMouseEvent()
	{
		if (m_InputHandler)
		{
			m_InputHandler->OnMouseEvent(m_MouseEvent);
		}
	}

	void OnKeyboardEvent()
	{
		if (m_InputHandler)
		{
			m_InputHandler->OnKeyboardEvent(m_KeyboardEvent);
		}
	}
private:
	uint32_t m_MinWidth = 0u;
	uint32_t m_MinHeight = 0u;
	uint32_t m_Width = 0u;
	uint32_t m_Height = 0u;
	EState m_State = EState::OnFocus;
	uint64_t m_Handle = 0u;
	IInputHandler* m_InputHandler = nullptr;
	MouseEvent m_MouseEvent;
	KeyboardEvent m_KeyboardEvent;
};

NAMESPACE_END(Gear)
