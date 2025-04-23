#pragma once

#include "MessageRouter.h"

#define MINIMAL_WINDOW_SIZE 32

struct WindowCreateInfo
{
	uint32_t Width = 1280u;
	uint32_t Height = 720u;
	uint32_t MinWidth = MINIMAL_WINDOW_SIZE;
	uint32_t MinHeight = MINIMAL_WINDOW_SIZE;
	std::string Title = "UnnamedApplication";

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_NVP(Width),
			CEREAL_NVP(Height),
			CEREAL_NVP(MinWidth),
			CEREAL_NVP(MinHeight),
			CEREAL_NVP(Title)
		);
	}
};

enum class EWindowMode : uint8_t
{
	Windowed,
	BorderlessFullscreen,
	ExclusiveFullscreen
};

class Window
{
public:
	Window(const WindowCreateInfo& CreateInfo, class BaseApplication* Owner);

	const uint32_t GetWidth() const { return m_Width; }
	const uint32_t GetHeight() const { return m_Height; }
	const void* GetHandle() const { return m_Handle; }
	const bool IsActivate() const { return m_Status == EWindowStatus::Activate; }
	const bool IsDestroyed() const { return m_Status == EWindowStatus::Destroyed; }

	void SetMode(EWindowMode Mode);
protected:
	friend class BaseApplication;

	void UpdateSize();
private:
	uint32_t m_MinWidth = 0u;
	uint32_t m_MinHeight = 0u;
	uint32_t m_Width = 0u;
	uint32_t m_Height = 0u;
	EWindowStatus m_Status = EWindowStatus::Activate;
	EWindowMode m_Mode = EWindowMode::Windowed;
	void* m_Handle = nullptr;
};

