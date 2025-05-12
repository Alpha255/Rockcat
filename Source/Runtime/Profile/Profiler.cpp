#include "Profile/Profiler.h"
#include "RHI/RHIDevice.h"

Profiler::Event::Event(const char* Name, EEventFlags Flags)
	: m_Name(Name)
	, m_Flags(Flags)
{
}

void Profiler::Event::Start(RHIDevice& /*Device*/)
{
	if (EnumHasAnyFlags(m_Flags, EEventFlags::Cpu))
	{
		m_CpuTime.MomentaryTime = 0.0f; // TODO: Get CPU time
	}
	if (EnumHasAnyFlags(m_Flags, EEventFlags::Gpu))
	{
		m_GpuTime.MomentaryTime = 0.0f; // TODO: Get GPU time
	}
}

void Profiler::Event::Stop(RHIDevice& /*Device*/)
{
}

Profiler::Profiler(RHIDevice& Device, bool Enabled)
	: m_Enabled(Enabled)
	, m_Device(Device)
{
}

void Profiler::Initialize()
{
}

void Profiler::Event::SetFlags(EEventFlags Flags)
{ 
	m_Flags = m_Flags | Flags;
}

void Profiler::Tick(float ElapsedSeconds)
{
	CalculateFrameTimeAndFPS(ElapsedSeconds);
}

void Profiler::CalculateFrameTimeAndFPS(float ElapsedSeconds)
{
	static float s_LastFrameTime = 0.0f;

	if (m_FrameCounter)
	{
		m_MomentaryFrameTime = ElapsedSeconds - s_LastFrameTime;
		s_LastFrameTime = ElapsedSeconds;
		m_MomentaryFPS = m_MomentaryFrameTime / m_FrameCounter;
	}

	m_FrameTimeCounter += ElapsedSeconds;
	++m_FrameCounter;
	++m_TotalFrameCounter;

	if (m_FrameTimeCounter > 1.0f)
	{
		m_AverageFrameTime = m_FrameTimeCounter / m_FrameCounter;
		m_FrameTimeCounter = 0.0f;
		m_FrameCounter = 0;
	}
}

Profiler::ScopedEvent Profiler::ScopeEvent(const char* Name, EEventFlags Flags)
{
	auto It = m_Events.find(Name);
	if (It == m_Events.end())
	{
		auto NewEvent = std::make_shared<Event>(Name, Flags);
		It = m_Events.insert(std::make_pair(std::string_view(NewEvent->GetName()), NewEvent)).first;
	}
	else
	{
		It->second->SetFlags(Flags);
	}

	return ScopedEvent(*It->second, m_Device);
}

#if 0

#include "Colorful/IRenderer/Profiler.h"

NAMESPACE_START(RHI)

namespace Profiler
{
	void Stats::Tick(float ElapsedSeconds)
	{
		m_TotalTime += ElapsedSeconds;
		UpdateAverageFrameTime(ElapsedSeconds);
	}

	void Stats::UpdateAverageFrameTime(float ElapsedSeconds)
	{
		m_FrameTime += ElapsedSeconds;
		++m_FrameCount;

		if (m_FrameTime > 1.0)
		{
			auto AverageFrameTime = m_FrameTime / m_FrameCount;
			m_FPS = 1.0f / AverageFrameTime;
			m_AverageFrameTime = AverageFrameTime * 1e3f;
			m_FrameTime = 0.0;
			m_FrameCount = 0;
		}
	}
}

NAMESPACE_END(RHI)

#endif