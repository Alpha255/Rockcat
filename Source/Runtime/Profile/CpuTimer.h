#pragma once

#include "Core/Definitions.h"

class CpuTimer
{
public:
	CpuTimer(float Speed = 1.0f, bool Start = true)
		: m_Speed(Speed)
		, m_State(Start ? EState::Tick : EState::Paused)
	{
		assert(Speed > 0.0f);
		m_LastTime = std::chrono::high_resolution_clock::now();
	}

	inline float GetElapsedMilliseconds()
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

	inline float GetElapsedSeconds()
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
			auto CurrentTime = std::chrono::high_resolution_clock::now();
			m_PausedTime = std::chrono::duration_cast<Seconds>(CurrentTime - m_LastTime).count();

			m_LastTime = std::chrono::high_resolution_clock::now();
			m_State = EState::Tick;
		}
	}
	
	inline void Pause()
	{
		if (m_State == EState::Tick)
		{
			m_State = EState::Paused;
			m_LastTime = std::chrono::high_resolution_clock::now();
		}
	}

	inline bool IsPaused() const { return m_State == EState::Paused; }

	using Seconds = std::chrono::duration<float, std::ratio<1, 1>>;
	using Milliseconds = std::chrono::duration<float, std::milli>;
protected:
	enum class EState
	{
		Tick,
		Paused
	};
private:
	std::chrono::high_resolution_clock::time_point m_LastTime;
	float m_Speed = 1.0f;
	float m_PausedTime = 0.0f;
	EState m_State = EState::Tick;
};

