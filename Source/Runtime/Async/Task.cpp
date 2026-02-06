#include "Async/Task.h"
#include "Core/ConsoleVariable.h"
#include "System/System.h"
#include "Services/SpdLogService.h"

thread_local TFTask::EThread t_ThreadTag = TFTask::EThread::WorkerThread;

ConsoleVariable<bool> CVarUseHyperThreading(
	"tf.use_hyper_threading",
	"Enable or disable hyper threading for taskflow executors.",
	false);

ConsoleVariable<bool> CVarUseSeperateGameThread(
	"tf.use_seperate_game_thread",
	"Enable or disable seperate game thread.",
	false);

ConsoleVariable<bool> CVarUseSeperateRenderThread(
	"tf.use_seperate_render_thread",
	"Enable or disable seperate render thread.",
	false);

ConsoleVariable<uint32_t> CVarNumForegroundThreads(
	"tf.num_foreground_threads",
	"Number of foreground threads.",
	2u);

struct ThreadPriorityScope
{
	ThreadPriorityScope(TFTask::EPriority Priority)
		: SkipPriorityChange(!TFTask::IsWorkerThread() || Priority == TFTask::EPriority::Normal)
	{
		if (!SkipPriorityChange)
		{
			System::SetThreadPriority(std::this_thread::get_id(), Priority);
		}
	}

	~ThreadPriorityScope()
	{
		if (!SkipPriorityChange)
		{
			System::SetThreadPriority(std::this_thread::get_id(), TFTask::EPriority::Normal);
		}
	}

	bool SkipPriorityChange;
};

class TFExecutorManager : public Singleton<TFExecutorManager>
{
public:
	void Initialize()
	{
		std::vector<size_t> WorkersInExecutor
		{
			CVarUseSeperateGameThread.Get(),
			CVarUseSeperateRenderThread.Get(),
			CVarNumForegroundThreads.Get(),
			TFTask::GetNumWorkerThreads()
		};

		m_Executors.reserve(WorkersInExecutor.size());

		for (auto& NumWorkers : WorkersInExecutor)
		{
			if (NumWorkers > 0u)
			{
				m_Executors.emplace_back(std::make_unique<tf::Executor>(NumWorkers));
			}
		}

		if (CVarNumForegroundThreads.Get())
		{
			auto Executor = GetExecutor(TFTask::EThread::WorkerThread, TFTask::EPriority::High);
			assert(Executor);

			tf::Taskflow Flow;
			std::vector<tf::Taskflow> SubFlows(CVarNumForegroundThreads.Get());

			for (auto& SubFlow : SubFlows)
			{
				SubFlow.emplace([]() {
					System::SetThreadPriority(std::this_thread::get_id(), TFTask::EPriority::High);

					std::stringstream Stream;
					Stream << std::this_thread::get_id();
					uint32_t ThreadID = std::stoul(Stream.str());
					LOG_INFO_CAT(LogTaskFlow, "Set foreground thread {} high priority", ThreadID);
				});
			}
			for (uint32_t Index = 0u; Index < SubFlows.size(); ++Index)
			{
				Flow.emplace([Executor, &SubFlow = SubFlows[Index]]() {
					Executor->corun(SubFlow);
				});
			}
			Executor->run(Flow).wait();
		}

		const uint32_t NumSeperateThreads = CVarUseSeperateGameThread.Get() + CVarUseSeperateRenderThread.Get() + CVarNumForegroundThreads.Get() > 0u;
		LOG_INFO_CAT(LogTaskFlow, "Create executors with {} seperate threads, {} worker threads, hyper threading is {}",
			NumSeperateThreads,
			TFTask::GetNumWorkerThreads(),
			CVarUseHyperThreading.Get() ? "enabled" : "disabled");
	}

	void Finalize()
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

	tf::Executor* GetExecutor(TFTask::EThread Thread, TFTask::EPriority Priority)
	{
		assert(Thread < TFTask::EThread::Num);

		if (!CVarUseSeperateGameThread.Get() && Thread == TFTask::EThread::GameThread)
		{
			return nullptr;
		}

		if (!CVarUseSeperateRenderThread.Get() && Thread == TFTask::EThread::RenderThread)
		{
			return nullptr;
		}

		size_t Index = static_cast<size_t>(Thread);
		if (Thread == TFTask::EThread::WorkerThread && Priority > TFTask::EPriority::Normal && CVarNumForegroundThreads.Get())
		{
			Index += 1u;
		}

		return m_Executors[Index].get();
	}
private:
	std::vector<std::unique_ptr<tf::Executor>> m_Executors;
};

void TFTask::Initialize()
{
	TFExecutorManager::Get().Initialize();
	InitializeThreadTags();

	LOG_INFO_CAT(LogTaskFlow, "Use taskflow @{}", tf::version());
}

void TFTask::Finalize()
{
	TFExecutorManager::Get().Finalize();
}

void TFTask::InitializeThreadTags()
{
	t_ThreadTag = TFTask::EThread::MainThread;

	if (CVarUseSeperateGameThread.Get())
	{
		Launch("tf.SetGameThreadTag", []() {
			t_ThreadTag = TFTask::EThread::GameThread;
		}, EThread::GameThread);
	}

	if (CVarUseSeperateRenderThread.Get())
	{
		Launch("tf.SetRenderThreadTag", []() {
			t_ThreadTag = TFTask::EThread::RenderThread;
		}, EThread::RenderThread);
	}
}

TFTaskEventPtr TFTask::DispatchTaskFlow(tf::Taskflow&& Flow, EThread Thread, EPriority Priority)
{
	if (auto Executor = TFExecutorManager::Get().GetExecutor(Thread, Priority))
	{
		return std::make_shared<TFTaskEvent>(std::move(Executor->run(std::forward<tf::Taskflow>(Flow))));
	}

	return nullptr;
}

uint32_t TFTask::GetNumWorkerThreads()
{
	static uint32_t s_NumWorkerThreads = 0u;

	if (!s_NumWorkerThreads)
	{
		uint32_t NumSeperateThreads = 0u;
		auto NumTotalThreads = System::GetHardwareConcurrencyThreadsCount(CVarUseHyperThreading.Get());

		NumSeperateThreads += CVarUseSeperateGameThread.Get() ? 1u : 0u;
		NumSeperateThreads += CVarUseSeperateRenderThread.Get() ? 1u : 0u;
		NumSeperateThreads -= CVarNumForegroundThreads.Get();

		s_NumWorkerThreads = static_cast<uint32_t>(NumTotalThreads - NumSeperateThreads);
	}

	return s_NumWorkerThreads;
}

bool TFTask::IsMainThread()
{
	return t_ThreadTag == EThread::MainThread;
}

bool TFTask::IsGameThread()
{
	return t_ThreadTag == EThread::GameThread;
}

bool TFTask::IsRenderThread()
{
	return t_ThreadTag == EThread::RenderThread;
}

bool TFTask::IsWorkerThread()
{
	return t_ThreadTag == EThread::WorkerThread;
}

void TFTask::AddPrerequisite(TFTask& Prerequisite)
{
	assert(IsSameThread(*this, Prerequisite) || !IsDispatched());

	Prerequisite.AddSubsequents(*this);

	std::lock_guard<std::mutex> Locker(m_Lock);
	m_Prerequisites.insert(&Prerequisite);
}

void TFTask::AddSubsequents(TFTask& Subsequent)
{
	if (!IsSameThread(*this, Subsequent))
	{
		std::lock_guard<std::mutex> Locker(m_Lock);
		m_Subsequents.insert(&Subsequent);
		Subsequent.AddRef();
	}
}

void TFTask::Execute()
{
	if (m_TaskFunc)
	{
		m_TaskFunc();
	}
}

void TFTask::TriggerSubsequents()
{
	std::lock_guard<std::mutex> Locker(m_Lock);
	for (auto Subsequent : m_Subsequents)
	{
		if (Subsequent->ReleaseRef())
		{
			Subsequent->Trigger();
		}
	}
}

void TFTask::Trigger()
{
	if (IsCanceled() || IsDispatched() || HasAnyRef())
	{
		return;
	}

	if (auto Executor = TFExecutorManager::Get().GetExecutor(m_Thread, m_Priority))
	{
		std::vector<tf::AsyncTask> PrerequisiteTasks;
		PrerequisiteTasks.reserve(m_Prerequisites.size());

		for (auto Prerequisite : m_Prerequisites)
		{
			if (Prerequisite)
			{
				Prerequisite->Trigger();

				if (auto LowLevelTask = Prerequisite->GetAsyncTask())
				{
					PrerequisiteTasks.emplace_back(tf::AsyncTask(*LowLevelTask));
				}
			}
		}

		m_TFAsyncTask = std::make_shared<TFAsyncTask>(std::move(Executor->dependent_async([this]() {
			ThreadPriorityScope ScopedTaskPriority(m_Priority);
			Execute();
			TriggerSubsequents();
		}, PrerequisiteTasks.begin(), PrerequisiteTasks.end())));
	}
}

bool TFTask::TryCancel()
{
	if (IsDispatched())
	{
		return false;
	}

	SetCanceled(true);
	return true;
}

//void TaskFlow::FrameSync(bool AllowOneFrameLag)
//{
//	if (m_SeparateRenderThread)
//	{
//		struct FrameSyncTask : public Task
//		{
//		protected:
//			virtual void ExecuteImpl() {}
//		};
//		static FrameSyncTask s_FrameSyncTask;
//
//		m_ThreadSyncEvents[m_ThreadEventIndex] = DispatchTask(s_FrameSyncTask, EThread::RenderThread);
//
//		if (AllowOneFrameLag)
//		{
//			m_ThreadEventIndex = (m_ThreadEventIndex + 1u) % m_ThreadSyncEvents.size();
//		}
//
//		if (m_ThreadSyncEvents[m_ThreadEventIndex])
//		{
//			if (m_SeparateGameThread)
//			{
//				auto GameThreadCompledEvent = DispatchTask(s_FrameSyncTask, EThread::GameThread);
//				do {
//					GameThreadCompledEvent->WaitForMilliseconds(1u);
//				} while (!GameThreadCompledEvent->IsCompleted());
//			}
//
//			m_ThreadSyncEvents[m_ThreadEventIndex]->Wait();
//			m_ThreadSyncEvents[m_ThreadEventIndex].reset();
//		}
//	}
//}