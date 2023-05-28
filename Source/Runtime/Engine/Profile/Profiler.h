#pragma once

#if 0

#include "Colorful/IRenderer/ICommandBuffer.h"

NAMESPACE_START(RHI)

namespace Profiler
{
	class ScopedDebugMarker
	{
	public:
		ScopedDebugMarker(ICommandBuffer* Command, const char8_t* Name, const Color& MarkerColor)
			: m_CommandBuffer(Command)
		{
			assert(Command && Name);
			m_CommandBuffer->BeginDebugMarker(Name, MarkerColor);
		}

		~ScopedDebugMarker()
		{
			m_CommandBuffer->EndDebugMarker();
		}
	protected:
	private:
		ICommandBuffer* m_CommandBuffer;
	};

	class IStopWatch
	{
	public:
		virtual ~IStopWatch() = default;
		virtual void Start() = 0;
		virtual void Stop() = 0;

		float32_t ElapsedMilliseconds() const
		{
			return m_Elapsed * 1e3f;
		}

		float32_t ElapsedSeconds() const
		{
			return m_Elapsed;
		}
	protected:
		float32_t m_Elapsed = 0.0;
	private:
	};

	class CpuStopWatch : public IStopWatch
	{
	public:
		CpuStopWatch()
		{
			m_Timer = std::make_shared<CpuTimer>(1.0f, false);
		}

		void Start() override final
		{
			m_Timer->Start();
		}

		void Stop() override final
		{
			m_Elapsed = m_Timer->ElapsedMilliseconds();
			m_Timer->Pause();
		}
	protected:
	private:
		std::shared_ptr<CpuTimer> m_Timer;
	};

	class GpuStopWatch : public IStopWatch
	{
	public:
		void Start() {}
		void Stop() {}
	};

	class ScopedCpuStopWatch : public CpuStopWatch
	{
	public:
		ScopedCpuStopWatch(const char8_t* Message)
			: m_Message(Message)
		{
			Start();
		}

		~ScopedCpuStopWatch()
		{
			Stop();
			
			LOG_INFO("{} takes {.2f}", m_Message.data(), ElapsedMilliseconds());
		}
	protected:
	private:
		std::string_view m_Message;
	};

	class Stats : public ITickable
	{
	public:
		class Event
		{
		public:
			Event(const char8_t* Name)
				: m_Name(Name)
			{
			}

			const char8_t* Name() const
			{
				return m_Name.data();
			}

			void Start();

			void Stop();

			float32_t CpuTime() const
			{
				return m_CpuTime;
			}

			float32_t CpuTimeAverage() const
			{
				return m_CpuTimeAverage;
			}

			float32_t GpuTime() const
			{
				return m_GpuTime;
			}

			float32_t GpuTimeAverage() const
			{
				return m_GpuTimeAverage;
			}
		protected:
		private:
			std::string_view m_Name;
			float32_t m_CpuTime = 0.0f;
			float32_t m_GpuTime = 0.0f;
			float32_t m_CpuTimeAverage = 0.0f;
			float32_t m_GpuTimeAverage = 0.0f;
			CpuStopWatch m_CpuStopWatch;
			GpuStopWatch m_GpuStopWatch;
		};

		class ScopedEvent
		{
		public:
			ScopedEvent(Event* Event)
				: m_Event(Event)
			{
				assert(Event);

				if (Stats::Get().Enabled())
				{
					m_Event->Start();
				}
			}

			~ScopedEvent()
			{
				if (Stats::Get().Enabled())
				{
					m_Event->Stop();
				}
			}
		protected:
		private:
			Event* m_Event = nullptr;
		};

		static Stats& Get();

		ScopedEvent CreateScopedEvent(const char8_t* Name);

		void Tick(float32_t ElapsedSeconds) override final;

		bool8_t Enabled() const
		{
			return m_Enabled;
		}

		void SetEnable(bool8_t Enabled)
		{
			m_Enabled = Enabled;
		}

		float32_t AverageFrameTime() const
		{
			return m_AverageFrameTime;
		}

		float32_t TotalFrameTime() const
		{
			return m_TotalTime;
		}

		float32_t FPS() const
		{
			return m_FPS;
		}
	protected:
		void UpdateAverageFrameTime(float32_t ElapsedSeconds);
	private:
		std::unordered_map<std::string_view, std::shared_ptr<Event>> m_Events;
		bool8_t m_Enabled = true;
		float32_t m_FrameTime = 0.0;
		float32_t m_TotalTime = 0.0;
		float32_t m_AverageFrameTime = 0.0;
		float32_t m_FPS = 0.0f;
		uint32_t m_FrameCount = 0u;
	};
}

#define SCOPED_RENDER_EVENT(Command, Name, MarkerColor) Profiler::ScopedDebugMarker RenderEvent_##Name(Command, #Name, MarkerColor);

#define SCOPED_STATS(Name) Profiler::Stats::Get().CreateScopedEvent(#Name);

#define SCOPED_CPU_EVENT(Message) Profiler::ScopedCpuStopWatch FILE_LINE(Message);

NAMESPACE_END(RHI)

#endif
