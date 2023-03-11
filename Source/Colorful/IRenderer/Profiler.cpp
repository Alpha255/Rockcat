#include "Colorful/IRenderer/Profiler.h"

NAMESPACE_START(RHI)

namespace Profiler
{
	void Stats::Event::Start()
	{
		m_CpuStopWatch.Start();
		m_GpuStopWatch.Start();
	}

	void Stats::Event::Stop()
	{
		m_CpuStopWatch.Stop();
		m_GpuStopWatch.Stop();

		m_CpuTime = m_CpuStopWatch.ElapsedMilliseconds();
		m_GpuTime = m_GpuStopWatch.ElapsedMilliseconds();
	}

	Stats& Stats::Get()
	{
		static Stats Instance;
		return Instance;
	}

	Stats::ScopedEvent Stats::CreateScopedEvent(const char8_t* Name)
	{
		auto& Scope = m_Events[Name];
		if (!Scope)
		{
			Scope = std::move(std::make_shared<Event>(Name));
		}

		return ScopedEvent(Scope.get());
	}

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