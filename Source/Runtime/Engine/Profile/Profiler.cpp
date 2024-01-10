#include "Runtime/Engine/Profile/Profiler.h"
#include "Runtime/Engine/RHI/RHIDevice.h"

Profiler::Event::Event(const char8_t* Name, EEventFlags Flags)
	: m_Name(Name)
	, m_Flags(Flags)
{
}

void Profiler::Event::Start(RHIDevice& Device)
{
}

void Profiler::Event::Stop(RHIDevice& Device)
{
}

Profiler::Profiler(RHIDevice& Device, bool8_t Enabled)
	: m_Enabled(Enabled)
	, m_Device(Device)
{
}

void Profiler::OnStartup()
{
}

void Profiler::Event::SetFlags(EEventFlags Flags)
{ 
	m_Flags = m_Flags | Flags;
}

void Profiler::Tick(float32_t ElapsedSeconds)
{
}

Profiler::ScopedEvent Profiler::ScopeEvent(const char8_t* Name, EEventFlags Flags)
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
	void Stats::Tick(float32_t ElapsedSeconds)
	{
		m_TotalTime += ElapsedSeconds;
		UpdateAverageFrameTime(ElapsedSeconds);
	}

	void Stats::UpdateAverageFrameTime(float32_t ElapsedSeconds)
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