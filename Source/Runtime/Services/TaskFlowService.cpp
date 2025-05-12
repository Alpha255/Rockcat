#include "Services/TaskFlowService.h"
#include "Services/SpdLogService.h"
#include "System.h"

TaskFlowService::TaskFlowService()
	: m_UseHyperThreading(false)
	, m_SeparateGameThread(false)
	, m_SeparateRenderThread(false)
	, m_NumThreads(0u)
	, m_NumWorkerThreads(0u)
	, m_NumSeperateThreads(0u)
	, m_NumForegroundThreads(2u)
{
}

void TaskFlowService::Initialize()
{
	Task::InitializeThreadTags();

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

	LOG_INFO("TaskFlow: Create taskflow executors with {} threads, {} seperate threads, {} worker threads, hyper threading is {}, taskflow version: {}",
		NumTotalThreads,
		m_NumSeperateThreads,
		m_NumWorkerThreads,
		m_UseHyperThreading ? "enabled" : "disabled",
		tf::version());
}

tf::Executor* TaskFlowService::GetExecutor(EThread Thread, Task::EPriority Priority)
{
	assert(Thread < EThread::Num);

	if (!m_SeparateGameThread && Thread == EThread::GameThread)
	{
		return nullptr;
	}

	if (!m_SeparateRenderThread && Thread == EThread::RenderThread)
	{
		return nullptr;
	}

	size_t ExecutorIndex = static_cast<size_t>(Thread);

	if (Thread == EThread::WorkerThread && Priority > Task::EPriority::Normal && m_NumForegroundThreads > 0u)
	{
		ExecutorIndex += 1u;
	}

	return m_Executors[ExecutorIndex].get();
}

void TaskFlowService::FrameSync(bool AllowOneFrameLag)
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

void TaskFlowService::Finalize()
{
	for (auto& Executor : m_Executors)
	{
		Executor->wait_for_all();
		Executor.reset();
	}
}
