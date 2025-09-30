#pragma once

#include "Core/Definitions.h"
#include "Core/Cereal.h"

#define MINIMAL_WINDOW_SIZE 32
#define WINDOW_CLASS_NAME L"RockcatWindowClass"

enum class EWindowMode
{
	Windowed,
	BorderlessFullscreen,
	ExclusiveFullscreen
};

struct WindowSettings
{
	uint32_t Width = 1280u;
	uint32_t Height = 720u;
	EWindowMode Mode = EWindowMode::Windowed;
	std::string Title = "RockcatWindow";

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_NVP(Width),
			CEREAL_NVP(Height),
			CEREAL_NVP_ENUM(EWindowMode, Mode),
			CEREAL_NVP(Title)
		);
	}
};

class Window
{
public:
	Window(const WindowSettings& Settings);

	uint32_t GetWidth() const { return m_Width; }
	uint32_t GetHeight() const { return m_Height; }
	const void* GetHandle() const { return m_Handle; }

	void SetMode(EWindowMode Mode);
protected:
	void UpdateSize(uint32_t Width, uint32_t Height);
private:
	uint32_t m_Width;
	uint32_t m_Height;
	EWindowMode m_Mode;
	void* m_Handle;
};

