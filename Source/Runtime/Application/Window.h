#pragma once

#include "Core/Definitions.h"

#define MINIMAL_WINDOW_SIZE 64u

enum class EWindowMode
{
	Windowed,
	BorderlessFullscreen,
	ExclusiveFullscreen
};

class Window
{
public:
	Window(uint32_t Width, uint32_t Height, const char* Title = nullptr)
		: m_Width(Width)
		, m_Height(Height)
		, m_Mode(EWindowMode::Windowed)
		, m_Handle(nullptr)
		, m_Title(Title)
	{
	}

	inline uint32_t GetWidth() const { return m_Width; }
	inline uint32_t GetHeight() const { return m_Height; }
	inline const void* GetHandle() const { return m_Handle; }

	inline EWindowMode GetMode() const { return m_Mode; }
	virtual void SetMode(EWindowMode Mode) = 0;

	inline bool IsFullscreen() const { return m_Mode != EWindowMode::Windowed; }
	inline bool IsWindowed() const { return m_Mode == EWindowMode::Windowed; }

	virtual void OnResize(uint32_t Width, uint32_t Height) = 0;

	virtual void SetTitle(const char* Title) = 0;
	std::string_view GetTitle() const { return m_Title; }
protected:
	uint32_t m_Width;
	uint32_t m_Height;
	EWindowMode m_Mode;
	void* m_Handle;
	std::string_view m_Title;
};

