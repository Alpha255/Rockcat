#pragma once

#include "Runtime/Engine/Tickable.h"

class Profiler : public ITickable
{
public:
	enum class EEventFlags
	{
		None = 0,
		Cpu = 0x1,
		Gpu = 0x2
	};

	class Event
	{
	public:
		Event(const char8_t* Name, EEventFlags Flags);

		const char8_t* GetName() { return m_Name.data(); }

		float32_t GetCpuTime() const { return m_CpuTime.MomentaryTime; }
		float32_t GetCpuTimeAverage() const { return m_CpuTime.AverageTime; }

		float32_t GetGpuTime() const { return m_GpuTime.MomentaryTime; }
		float32_t GetGpuTimeAverage() const { return m_GpuTime.AverageTime; }

		void Start(class RHIDevice& Device);
		void Stop(class RHIDevice& Device);
	protected:
		struct EventTime
		{
			float32_t MomentaryTime = 0.0f;
			float32_t AverageTime = 0.0f;
			float32_t TotalTime = 0.0f;
		};
	private:
		std::string_view m_Name;
		EventTime m_CpuTime;
		EventTime m_GpuTime;
		EEventFlags m_Flags;
	};

	class ScopedEvent
	{
	public:
		ScopedEvent(Event& InEvent, class RHIDevice& Device)
			: m_Event(InEvent)
			, m_Device(Device)
		{
			m_Event.Start(m_Device);
		}

		~ScopedEvent()
		{
			m_Event.Stop(m_Device);
		}
	private:
		Event& m_Event;
		class RHIDevice& m_Device;
	};

	Profiler(class RHIDevice& Device, bool8_t Enabled = true);

	ScopedEvent ScopeEvent(const char8_t* Name, EEventFlags Flags);

	bool8_t IsEnabled() const { return m_Enabled; }
	void SetEnabled(bool8_t Enable) { m_Enabled = Enable; }

	void Tick(float32_t ElapsedSeconds) override final;
protected:
private:
	std::unordered_map<std::string_view, std::shared_ptr<Event>> m_Events;
	bool8_t m_Enabled;
	class RHIDevice& m_Device;
};

ENUM_FLAG_OPERATORS(Profiler::EEventFlags)
