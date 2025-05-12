#pragma once

#include "EventRouter.h"

#define MINIMAL_WINDOW_SIZE 32

enum class EWindowMode
{
	Windowed,
	BorderlessFullscreen,
	ExclusiveFullscreen
};

struct WindowCreateInfo
{
	uint32_t Width = 1280u;
	uint32_t Height = 720u;
	uint32_t MinWidth = MINIMAL_WINDOW_SIZE;
	uint32_t MinHeight = MINIMAL_WINDOW_SIZE;
	EWindowMode Mode = EWindowMode::Windowed;
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

class Window
{
public:
	Window(const WindowCreateInfo& CreateInfo);

	const uint32_t GetWidth() const { return m_Width; }
	const uint32_t GetHeight() const { return m_Height; }
	const uint32_t GetMinWidth() const { return m_MinWidth; }
	const uint32_t GetMinHeight() const { return m_MinHeight; }
	const void* GetHandle() const { return m_Handle; }
	const bool IsActivate() const { return m_Status == EWindowStatus::Activate; }
	const bool IsDestroyed() const { return m_Status == EWindowStatus::Destroyed; }

	void SetMode(EWindowMode Mode);
protected:
	friend class MessageProcessor;

	inline void SetStatus(EWindowStatus Status) { m_Status = Status; }

	void UpdateSize();
private:
	uint32_t m_MinWidth;
	uint32_t m_MinHeight;
	uint32_t m_Width;
	uint32_t m_Height;
	EWindowMode m_Mode;
	EWindowStatus m_Status;
	void* m_Handle;
	std::unique_ptr<MessageProcessor> m_MessageProcessor;
};

