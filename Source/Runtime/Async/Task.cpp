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

ConsoleVariable<bool> CVarUseSeperateRHIThread(
	"tf.use_seperate_rhi_thread",
	"Enable or disable seperate rhi thread.",
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
			TFTask::GetNumWorkerThreads(),
			CVarNumForegroundThreads.Get()
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
			for (uint32_t Index = 0u; Index < CVarNumForegroundThreads.Get(); ++Index)
			{
				Flow.emplace([]() {
					const std::thread::id ThreadID = std::this_thread::get_id();

					System::SetThreadPriority(ThreadID, TFTask::EPriority::High);

					std::stringstream Stream;
					Stream << ThreadID;
					LOG_INFO_CAT(LogTaskFlow, "Set foreground thread {} high priority", std::stoul(Stream.str()));
				});
			}
			Executor->run(Flow);
			Executor->wait_for_all();
		}

		const uint32_t NumSeperateThreads = CVarUseSeperateGameThread.Get() + CVarUseSeperateRenderThread.Get() + CVarNumForegroundThreads.Get();
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

		static Array<size_t, TFTask::EThread> s_ExecutorIndices {
			static_cast<size_t>(TFTask::EThread::GameThread),
			static_cast<size_t>(TFTask::EThread::RenderThread) - !CVarUseSeperateGameThread.Get(),
			static_cast<size_t>(TFTask::EThread::WorkerThread) - !CVarUseSeperateGameThread.Get() - !CVarUseSeperateRenderThread.Get()
		};

		const bool IsHighPriority = (Priority > TFTask::EPriority::Normal) ||
			(Thread == TFTask::EThread::GameThread && !CVarUseSeperateGameThread.Get()) ||
			(Thread == TFTask::EThread::RenderThread && !CVarUseSeperateRenderThread.Get());

		return m_Executors[s_ExecutorIndices[static_cast<size_t>(Thread)] + IsHighPriority].get();
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
		}, EThread::GameThread)->Wait();
	}

	if (CVarUseSeperateRenderThread.Get())
	{
		Launch("tf.SetRenderThreadTag", []() {
			t_ThreadTag = TFTask::EThread::RenderThread;
		}, EThread::RenderThread)->Wait();
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
	assert(IsSameExecutor(*this, Prerequisite) || !IsDispatched());

	Prerequisite.AddSubsequents(*this);

	std::lock_guard<std::mutex> Locker(m_Lock);
	m_Prerequisites.insert(&Prerequisite);
}

void TFTask::AddSubsequents(TFTask& Subsequent)
{
	if (!IsSameExecutor(*this, Subsequent))
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

bool TFTask::Trigger()
{
	if (IsCanceled() || IsDispatched() || HasAnyRef())
	{
		return false;
	}

	if (auto Executor = TFExecutorManager::Get().GetExecutor(m_Thread, m_Priority))
	{
		std::vector<tf::AsyncTask> PrerequisiteTasks;
		PrerequisiteTasks.reserve(m_Prerequisites.size());

		for (auto Prerequisite : m_Prerequisites)
		{
			if (Prerequisite && Prerequisite->Trigger())
			{
				if (auto LowLevelTask = Prerequisite->GetAsyncTask())
				{
					PrerequisiteTasks.emplace_back(tf::AsyncTask(*LowLevelTask));
				}
			}
		}

		m_TFAsyncTask = std::make_shared<TFAsyncTask>(std::move(Executor->dependent_async([this]() {
			//ThreadPriorityScope ScopedTaskPriority(m_Priority);
			Execute();
			TriggerSubsequents();
		}, PrerequisiteTasks.begin(), PrerequisiteTasks.end())));

		return true;
	}

	return false;
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

TFTask::~TFTask()
{
	if (IsDispatched() && !IsCompleted())
	{
		LOG_WARNING_CAT(LogTaskFlow, "Unexpected wait by task: {}", GetName().Get());
		Wait();
	}
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