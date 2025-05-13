#pragma once

#include "Core/Module.h"
#include "Core/Tickable.h"
#include "Profile/CpuTimer.h"

class StatEvent
{
public:
	enum class EEventFlags
	{
		None,
		Cpu = 1 << 0,
		Gpu = 1 << 1,
		All = Cpu | Gpu
	};

	StatEvent(const char* Name, EEventFlags Flags);

	const char* GetName() { return m_Name.data(); }

	float GetCpuTime() const { return m_CpuTime.Time; }
	float GetAverageCpuTime() const { return m_CpuTime.AverageTime; }

	float GetGpuTime() const { return m_GpuTime.Time; }
	float GetAverageGpuTime() const { return m_GpuTime.AverageTime; }

	void SetFlags(EEventFlags Flags);

	void Start();
	void Stop();
protected:
private:
	struct EventTime
	{
		float Time = 0.0f;
		float AverageTime = 0.0f;
		float TotalTime = 0.0f;
	};

	std::string_view m_Name;
	EventTime m_CpuTime;
	EventTime m_GpuTime;
	EEventFlags m_Flags;

	std::unique_ptr<CpuTimer> m_CpuTimer;
};

//class ScopedEvent
//{
//public:
//	ScopedEvent(Event& InEvent)
//		: m_Event(InEvent)
//	{
//	}
//
//	~ScopedEvent()
//	{
//		m_Event.Stop();
//	}
//private:
//	Event& m_Event;
//};

class Stats : public ITickable, public IService<Stats>
{
public:
	Stats()
		: ITickable(false)
	{
	}

	inline bool IsEnabled() const { return m_Enabled; }
	inline void SetEnable(bool Enable) { m_Enabled = Enable; }

	inline float GetFrameTime() const { return m_FrameTime; }
	inline float GetAverageFrameTime() const { return m_AverageFrameTime; }

	inline float GetFPS() const { return m_FPS; }
	inline float GetAverageFPS() const { return m_AverageFPS; }

	inline uint64_t GetNumFrame() const { return m_NumFrame; }

	inline uint32_t GetNumPrimitives() const { return m_NumPrimitives.load(std::memory_order_relaxed); }
	inline uint32_t GetNumDraws() const { return m_NumDraw.load(std::memory_order_relaxed); }

	inline void DrawPrimitves(uint32_t NumPrimitives, uint32_t NumDraws)
	{
		m_NumPrimitives.fetch_add(NumPrimitives, std::memory_order_relaxed);
		m_NumDraw.fetch_add(NumDraws, std::memory_order_relaxed);
	}

	void Tick(float ElapsedSeconds) override final;
protected:
private:
	void CalcFrameTime(float ElapsedSeconds);

	std::unordered_map<std::string_view, std::unique_ptr<StatEvent>> m_Events;
	bool m_Enabled = true;

	float m_FrameTime = 0.0f;
	float m_AverageFrameTime = 0.0f;

	float m_FPS = 0.0f;
	float m_AccumFPS = 0.0f;
	float m_AverageFPS = 0.0f;

	float m_AccumFrameTime = 0.0f;
	uint32_t m_AccumFrameNum = 1u;

	uint64_t m_NumFrame = 0u;

	std::atomic<uint32_t> m_NumPrimitives = 0u;
	std::atomic<uint32_t> m_NumDraw = 0u;
};

ENUM_FLAG_OPERATORS(StatEvent::EEventFlags)
