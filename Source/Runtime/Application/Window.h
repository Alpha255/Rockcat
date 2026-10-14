#pragma once

#include "Core/Cereal.h"

#define MINIMAL_WINDOW_SIZE 32
#define WINDOW_CLASS_NAME L"RockcatWindowClass"

enum class EWindowMode
{
	Windowed,
	BorderlessFullscreen,
	ExclusiveFullscreen
};

/// Must run once, before the first window is created: Windows fixes the DPI awareness of a process
/// the first time it is queried and never lets it be relaxed afterwards.
void InitializeDpiAwareness();

struct WindowSettings
{
	uint32_t Width = 1280u;
	uint32_t Height = 720u;
	EWindowMode Mode = EWindowMode::Windowed;
	string Title = "RockcatWindow";

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

	/// Device pixels per logical pixel of this window, 1.0 at 96 DPI (100%). Width/Height above are
	/// device pixels: once the process is per monitor DPI aware the OS reports the client area in
	/// those, and everything that heads for the GPU (the swap chain above all) matches them 1:1.
	/// Only the UI density differs, see ImGuiConfiguration::SetDisplayScale().
	float GetDpiScale() const;

	void SetMode(EWindowMode Mode);
	inline EWindowMode GetMode() const { return m_Mode; }
	inline bool IsFullscreen() const { return m_Mode != EWindowMode::Windowed; }

	/// Driven by the platform layer whenever the client area really changed size: a window resize,
	/// or a density change that reshaped it. (0, 0) asks for the current size to be re-read from
	/// the OS. Any other size unit than logical would leak up into ImGui's layout.
	void UpdateSize(uint32_t Width, uint32_t Height);
private:
	uint32_t m_Width;
	uint32_t m_Height;
	EWindowMode m_Mode;
	void* m_Handle;
};

