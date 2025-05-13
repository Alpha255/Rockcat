#include "Profile/Stats.h"

StatEvent::StatEvent(const char* Name, EEventFlags Flags)
	: m_Name(Name)
	, m_Flags(Flags)
{
}

void StatEvent::Start()
{
	if (EnumHasAnyFlags(m_Flags, EEventFlags::Cpu))
	{
		m_CpuTime.Time = 0.0f; // TODO: Get CPU time
		if (!m_CpuTimer)
		{
			m_CpuTimer = std::make_unique<CpuTimer>();
		}
		else
		{
			m_CpuTimer->Start();
		}
	}
	if (EnumHasAnyFlags(m_Flags, EEventFlags::Gpu))
	{
		m_GpuTime.Time = 0.0f; // TODO: Get GPU time
	}
}

void StatEvent::Stop()
{
	if (EnumHasAnyFlags(m_Flags, EEventFlags::Cpu))
	{
		m_CpuTimer->Pause();
	}
}

void StatEvent::SetFlags(EEventFlags Flags)
{ 
	m_Flags = m_Flags | Flags;
}

void Stats::Tick(float ElapsedSeconds)
{
	CalcFrameTime(ElapsedSeconds);

	m_NumPrimitives.store(0u, std::memory_order_relaxed);
	m_NumDraw.store(0u, std::memory_order_relaxed);
}

void Stats::CalcFrameTime(float ElapsedSeconds)
{
	m_FrameTime = ElapsedSeconds;
	m_FPS = 1.0f / m_FrameTime;

	m_AccumFrameTime += m_FrameTime;
	m_AccumFPS += m_FPS;

	++m_AccumFrameNum;
	++m_NumFrame;

	if (m_AccumFrameTime > 1.0f)
	{
		m_AverageFrameTime = m_AccumFrameTime / m_AccumFrameNum;
		m_AverageFPS = m_AccumFPS / m_AccumFrameNum;

		m_AccumFrameTime = 0.0f;
		m_AccumFPS = 0.0f;
		m_AccumFrameNum = 0u;
	}
}

//Profiler::ScopedEvent Profiler::ScopeEvent(const char* Name, EEventFlags Flags)
//{
//	auto It = m_Events.find(Name);
//	if (It == m_Events.end())
//	{
//		auto NewEvent = std::make_shared<Event>(Name, Flags);
//		It = m_Events.insert(std::make_pair(std::string_view(NewEvent->GetName()), NewEvent)).first;
//	}
//	else
//	{
//		It->second->SetFlags(Flags);
//	}
//
//	return ScopedEvent(*It->second);
//}