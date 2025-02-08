#pragma once

#include "Core/InputState.h"

#define MINIMAL_WINDOW_SIZE 32

struct WindowCreateInfo
{
	uint32_t Width = 1280u;
	uint32_t Height = 720u;
	uint32_t MinWidth = MINIMAL_WINDOW_SIZE;
	uint32_t MinHeight = MINIMAL_WINDOW_SIZE;
	bool FreezeWhenInactive = true;
	std::string Title = "UnnamedApplication";

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_NVP(Width),
			CEREAL_NVP(Height),
			CEREAL_NVP(MinWidth),
			CEREAL_NVP(MinHeight),
			CEREAL_NVP(FreezeWhenInactive),
			CEREAL_NVP(Title)
		);
	}
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

	enum class EMode : uint8_t
	{
		Windowed,
		BorderlessFullscreen,
		ExclusiveFullscreen
	};

	Window(const WindowCreateInfo& CreateInfo, IInputHandler* InputHandler);

	const uint32_t GetWidth() const { return m_Width; }
	const uint32_t GetHeight() const { return m_Height; }
	const void* GetHandle() const { return m_Handle; }
	const bool IsActive() const { return m_State == EState::OnFocus; }
	const bool IsDestroyed() const { return m_State == EState::Destroyed; }
	const EState GetState() const { return m_State; }

	void SetMode(EMode Mode);

	void ProcessMessage(uint32_t Message, size_t WParam, intptr_t LParam);

	void PollMessage();
protected:
	void UpdateSize(bool Signal = false);

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
	EMode m_Mode = EMode::Windowed;
	void* m_Handle = nullptr;
	IInputHandler* m_InputHandler = nullptr;
	MouseEvent m_MouseEvent;
	KeyboardEvent m_KeyboardEvent;
};

