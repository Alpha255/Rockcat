#pragma once

#include "Core/Definitions.h"

NAMESPACE_START(Gear)

class CpuTimer
{
public:
	CpuTimer(float32_t Speed = 1.0f, bool8_t Start = true)
		: m_Speed(Speed)
		, m_State(Start ? EState::Tick : EState::Paused)
	{
		assert(Speed > 0.0f);
		m_LastTime = std::chrono::high_resolution_clock::now();
	}

	inline float32_t ElapsedMilliseconds()
	{
		if (m_State == EState::Tick)
		{
			auto CurrentTime = std::chrono::high_resolution_clock::now();
			auto Elapsed = std::chrono::duration_cast<Milliseconds>(CurrentTime - m_LastTime).count();
			m_LastTime = CurrentTime;
			return Elapsed * m_Speed;
		}

		return 0.0;
	}

	inline float32_t ElapsedSeconds()
	{
		if (m_State == EState::Tick)
		{
			auto CurrentTime = std::chrono::high_resolution_clock::now();
			auto Elapsed = std::chrono::duration_cast<Seconds>(CurrentTime - m_LastTime).count();
			m_LastTime = CurrentTime;
			return Elapsed * m_Speed;
		}

		return 0.0;
	}

	inline void Start()
	{
		if (m_State == EState::Paused)
		{
			m_LastTime = std::chrono::high_resolution_clock::now();
			m_State = EState::Tick;
		}
	}
	
	inline void Pause()
	{
		if (m_State == EState::Tick)
		{
			m_State = EState::Paused;
		}
	}

	using Seconds = std::chrono::duration<float32_t, std::ratio<1, 1>>;
	using Milliseconds = std::chrono::duration<float32_t, std::milli>;
protected:
	enum class EState
	{
		Tick,
		Paused
	};
private:
	std::chrono::high_resolution_clock::time_point m_LastTime;
	float32_t m_Speed = 1.0f;
	float32_t m_ElapsedTime = 0.0f;
	EState m_State = EState::Tick;
};

NAMESPACE_END(Gear)
