#include "Services/TaskFlowService.h"
#include "Services/SpdLogService.h"
#include "System/System.h"

void TaskFlow::Initialize()
{
	TaskFlowTask::InitializeThreadTags();

	LOG_INFO_CAT(LogTaskFlow, "Use taskflow @{}", tf::version());

	m_Executors.resize(static_cast<size_t>(EThread::Num) + 1u);

	auto NumTotalThreads = System::GetHardwareConcurrencyThreadsCount(m_UseHyperThreading);

	if (m_SeparateGameThread)
	{
		m_Executors[static_cast<size_t>(EThread::GameThread)].reset(new tf::Executor(1u));
		m_NumSeperateThreads += 1u;
	}

	if (m_SeparateRenderThread)
	{
		m_Executors[static_cast<size_t>(EThread::RenderThread)].reset(new tf::Executor(1u));
		m_NumSeperateThreads += 1u;
	}

	m_NumWorkerThreads = static_cast<uint8_t>(NumTotalThreads - m_NumSeperateThreads);

	if (m_NumForegroundThreads > 0u)
	{
		uint8_t NumBackgroundThreads = m_NumWorkerThreads - m_NumForegroundThreads;
		m_Executors[static_cast<size_t>(EThread::WorkerThread)].reset(new tf::Executor(m_NumForegroundThreads));
		m_Executors[static_cast<size_t>(EThread::WorkerThread) + 1u].reset(new tf::Executor(NumBackgroundThreads));

		auto ForegroundExecutor = m_Executors[static_cast<size_t>(EThread::WorkerThread)].get();
		tf::Taskflow Flow;
		std::vector<tf::Taskflow> SubFlows(m_NumForegroundThreads);
		for (auto& SubFlow : SubFlows)
		{
			SubFlow.emplace([]() {
				System::SetThreadPriority(std::this_thread::get_id(), Task::EPriority::High);

				std::stringstream Stream;
				Stream << std::this_thread::get_id();
				uint32_t ThreadID = std::stoul(Stream.str());
				LOG_INFO_CAT(LogTaskFlow, "Set foreground thread {} high priority", ThreadID);
			});
		}
		for (uint32_t Index = 0u; Index < SubFlows.size(); ++Index)
		{
			Flow.emplace([ForegroundExecutor, &SubFlow = SubFlows[Index]]() {
				ForegroundExecutor->corun(SubFlow);
			});
		}
		ForegroundExecutor->run(Flow).wait();
	}
	else
	{
		m_Executors[static_cast<size_t>(EThread::WorkerThread)].reset(new tf::Executor(m_NumWorkerThreads));
	}

	LOG_INFO_CAT(LogTaskFlow, "Create taskflow executors with {} threads, {} seperate threads, {} worker threads, hyper threading is {}",
		NumTotalThreads,
		m_NumSeperateThreads,
		m_NumWorkerThreads,
		m_UseHyperThreading ? "enabled" : "disabled");
}

void TaskFlow::FrameSync(bool AllowOneFrameLag)
{
	if (m_SeparateRenderThread)
	{
		struct FrameSyncTask : public Task
		{
		protected:
			virtual void ExecuteImpl() {}
		};
		static FrameSyncTask s_FrameSyncTask;

		m_ThreadSyncEvents[m_ThreadEventIndex] = DispatchTask(s_FrameSyncTask, EThread::RenderThread);

		if (AllowOneFrameLag)
		{
			m_ThreadEventIndex = (m_ThreadEventIndex + 1u) % m_ThreadSyncEvents.size();
		}

		if (m_ThreadSyncEvents[m_ThreadEventIndex])
		{
			if (m_SeparateGameThread)
			{
				auto GameThreadCompledEvent = DispatchTask(s_FrameSyncTask, EThread::GameThread);
				do {
					GameThreadCompledEvent->WaitForMilliseconds(1u);
				} while (!GameThreadCompledEvent->IsCompleted());
			}

			m_ThreadSyncEvents[m_ThreadEventIndex]->Wait();
			m_ThreadSyncEvents[m_ThreadEventIndex].reset();
		}
	}
}

void TaskFlow::Finalize()
{
	for (auto& Executor : m_Executors)
	{
		if (Executor)
		{
			Executor->wait_for_all();
			Executor.reset();
		}
	}
}
