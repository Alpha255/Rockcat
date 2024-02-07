#pragma once

#include "Engine/Tickable.h"
#include "Core/Module.h"

class Profiler : public ITickable, public IService<Profiler>
{
public:
	enum class EEventFlags
	{
		None = 0,
		Cpu = 0x1,
		Gpu = 0x2,
		All = 0x3
	};

	class Event
	{
	public:
		Event(const char* Name, EEventFlags Flags);

		const char* GetName() { return m_Name.c_str(); }

		float GetCpuTime() const { return m_CpuTime.MomentaryTime; }
		float GetCpuTimeAverage() const { return m_CpuTime.AverageTime; }

		float GetGpuTime() const { return m_GpuTime.MomentaryTime; }
		float GetGpuTimeAverage() const { return m_GpuTime.AverageTime; }

		void SetFlags(EEventFlags Flags);

		void Start(class RHIDevice& Device);
		void Stop(class RHIDevice& Device);
	protected:
		struct EventTime
		{
			float MomentaryTime = 0.0f;
			float AverageTime = 0.0f;
			float TotalTime = 0.0f;
		};
	private:
		std::string m_Name;
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
		}

		~ScopedEvent()
		{
			m_Event.Stop(m_Device);
		}
	private:
		Event& m_Event;
		class RHIDevice& m_Device;
	};

	Profiler(class RHIDevice& Device, bool Enabled = true);

	void OnStartup() override final;

	ScopedEvent ScopeEvent(const char* Name, EEventFlags Flags);

	bool IsEnabled() const { return m_Enabled; }
	void SetEnabled(bool Enable) { m_Enabled = Enable; }

	void Tick(float ElapsedSeconds) override final;
private:
	std::unordered_map<std::string_view, std::shared_ptr<Event>> m_Events;
	bool m_Enabled = true;
	class RHIDevice& m_Device;
};

ENUM_FLAG_OPERATORS(Profiler::EEventFlags)

#define PROFILE_EVENT_FLAGS(Flags, NameFormat, ...) \
	static std::string CAT(__ProfileEventName, __LINE__) = StringUtils::Format(NameFormat, __VA_ARGS__); \
	Profiler::ScopedEvent CAT(__ProfileEvent, __LINE__) = Profiler::Get().ScopeEvent(CAT(__ProfileEventName, __LINE__).c_str(), Flags);

#define PROFILE_EVENT(NameFormat, ...) PROFILE_EVENT_FLAGS(Profiler::EEventFlags::All, NameFormat, __VA_ARGS__)
#define PROFILE_CPU_EVENT(NameFormat, ...) PROFILE_EVENT_FLAGS(Profiler::EEventFlags::Cpu, NameFormat, __VA_ARGS__)
#define PROFILE_GPU_EVENT(NameFormat, ...) PROFILE_EVENT_FLAGS(Profiler::EEventFlags::Gpu, NameFormat, __VA_ARGS__)
