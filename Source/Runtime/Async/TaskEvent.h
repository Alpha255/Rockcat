#pragma once

#include "Core/Definitions.h"
#include <future>

enum class ETFTaskThread
{
	GameThread,
	RenderThread,
	WorkerThread,
	Num
};

enum class ETFTaskPriority : uint8_t
{
	Low,
	Normal,
	High,
	Critical
};

enum class ETFTaskState : uint8_t
{
	None,
	Dispatched,
	Canceled,
	Completed
};

class TFTaskEvent
{
public:
	TFTaskEvent() = delete;
	TFTaskEvent(const TFTaskEvent&) = delete;
	TFTaskEvent(TFTaskEvent&& Other) noexcept = default;
	TFTaskEvent& operator=(const TFTaskEvent&) = delete;
	TFTaskEvent& operator=(TFTaskEvent&& Other) noexcept = default;

	TFTaskEvent(std::future<void>&& Future) noexcept
		: m_Future(std::move(Future))
	{
	}

	inline void Wait()
	{
		if (m_Future.valid())
		{
			m_Future.get();
		}
	}

	inline void WaitForSeconds(size_t Seconds)
	{
		if (m_Future.valid())
		{
			m_Future.wait_for(std::chrono::seconds(Seconds));
		}
	}

	inline void WaitForMilliseconds(size_t Milliseconds)
	{
		if (m_Future.valid())
		{
			m_Future.wait_for(std::chrono::milliseconds(Milliseconds));
		}
	}
private:
	std::future<void> m_Future;
};
using TFTaskEventPtr = std::shared_ptr<TFTaskEvent>;