#include "Engine/Services/TaskFlowService.h"
#include "Engine/Services/SpdLogService.h"
#include "Core/PlatformMisc.h"

TaskFlowService::TaskFlowService()
	: m_UseHyperThreading(false)
	, m_SeparateGameThread(false)
	, m_SeparateRenderThread(false)
	, m_NumWorkThreads(0u)
{
}

void TaskFlowService::OnStartup()
{
	size_t NumSeperateThread = 0u;
	auto NumTotalThreads = PlatformMisc::GetHardwareConcurrencyThreadsCount(m_UseHyperThreading);

	if (m_SeparateGameThread)
	{
		m_Executors[EThread::GameThread].reset(new tf::Executor(1u));
		NumSeperateThread += 1u;
	}

	if (m_SeparateRenderThread)
	{
		m_Executors[EThread::RenderThread].reset(new tf::Executor(1u));
		NumSeperateThread += 1u;
	}

	m_NumWorkThreads = static_cast<uint8_t>(NumTotalThreads - NumSeperateThread);
	m_Executors[EThread::WorkerThread].reset(new tf::Executor(m_NumWorkThreads));

	LOG_INFO("TaskFlow: Create taskflow executors with {} threads, {} seperate threads, {} worker threads, hyper threading is {}, taskflow version: {}",
		NumTotalThreads,
		NumSeperateThread,
		m_NumWorkThreads,
		m_UseHyperThreading ? "enabled" : "disabled",
		tf::version());
}

void TaskFlowService::FrameSync(bool AllowOneFrameLag)
{
	if (m_SeparateRenderThread)
	{
		struct FrameSyncTask : public Task
		{
			virtual void Execute() {}
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

void TaskFlowService::UpdateTaskFlows()
{
	m_TaskFlows.remove_if([](TaskFlow& Flow) {
		return Flow.IsCompleted();
	});
}

void TaskFlowService::OnShutdown()
{
	for (auto& Executor : m_Executors)
	{
		Executor->wait_for_all();
		Executor.reset();
	}
}
