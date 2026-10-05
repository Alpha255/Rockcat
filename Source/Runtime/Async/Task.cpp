#include "Async/Task.h"
#include "Core/ConsoleVariable.h"
#include "Misc/PlatformMisc.h"
#include "Core/SpdLogging.h"

#include <barrier>

DEFINE_LOGGER_CATEGORY(LogTaskFlow);

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

struct SetThreadPriorityScoped
{
	SetThreadPriorityScoped(TFTask::EPriority Priority)
		: SkipPriorityChange(!TFTask::IsWorkerThread() || Priority == TFTask::EPriority::Normal)
	{
		if (!SkipPriorityChange)
		{
			PlatformMisc::SetThreadPriority(std::this_thread::get_id(), Priority);
		}
	}

	~SetThreadPriorityScoped()
	{
		if (!SkipPriorityChange)
		{
			PlatformMisc::SetThreadPriority(std::this_thread::get_id(), TFTask::EPriority::Normal);
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

		if (const uint32_t NumForegroundThreads = CVarNumForegroundThreads.Get())
		{
			if (auto Executor = GetExecutor(TFTask::EThread::WorkerThread, TFTask::EPriority::High))
			{
				// A thread runs one task at a time, so blocking all N here forces the priority change above onto N distinct workers.
				std::barrier Barrier(static_cast<ptrdiff_t>(NumForegroundThreads));

				tf::Taskflow Flow;
				for (uint32_t Index = 0u; Index < NumForegroundThreads; ++Index)
				{
					Flow.emplace([&Barrier]() {
						PlatformMisc::SetThreadPriority(std::this_thread::get_id(), TFTask::EPriority::High);
						Barrier.arrive_and_wait();
					});
				}

				Executor->run(Flow);
				Executor->wait_for_all();

				LOG_INFO(LogTaskFlow, "Set {} foreground thread(s) to high priority", NumForegroundThreads);
			}
		}

		const uint32_t NumSeperateThreads = CVarUseSeperateGameThread.Get() + CVarUseSeperateRenderThread.Get() + CVarNumForegroundThreads.Get();
		LOG_INFO(LogTaskFlow, "Create executors with {} seperate threads, {} worker threads, hyper threading is {}",
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

		m_Executors.clear();
	}

	tf::Executor* GetExecutor(TFTask::EThread Thread, TFTask::EPriority Priority)
	{
		assert(Thread < TFTask::EThread::Num);

		static const Array<size_t, TFTask::EThread> s_ExecutorIndices {
			static_cast<size_t>(TFTask::EThread::GameThread),
			static_cast<size_t>(TFTask::EThread::RenderThread) - !CVarUseSeperateGameThread.Get(),
			static_cast<size_t>(TFTask::EThread::WorkerThread) - !CVarUseSeperateGameThread.Get() - !CVarUseSeperateRenderThread.Get()
		};

		const size_t ThreadIndex = static_cast<size_t>(Thread);
		assert(ThreadIndex < s_ExecutorIndices.size());

		const bool IsHighPriority = (Priority > TFTask::EPriority::Normal) ||
			(Thread == TFTask::EThread::GameThread && !CVarUseSeperateGameThread.Get()) ||
			(Thread == TFTask::EThread::RenderThread && !CVarUseSeperateRenderThread.Get());

		const size_t ExecutorIndex = s_ExecutorIndices[ThreadIndex] + (IsHighPriority ? 1u : 0u);
		assert(ExecutorIndex < m_Executors.size());

		return m_Executors[ExecutorIndex].get();
	}
private:
	std::vector<std::unique_ptr<tf::Executor>> m_Executors;
};

void TFTask::Initialize()
{
	TFExecutorManager::Get().Initialize();
	InitializeThreadTags();

	LOG_INFO(LogTaskFlow, "Use taskflow @{}", tf::version());
}

void TFTask::Finalize()
{
	TFExecutorManager::Get().Finalize();
}

void TFTask::InitializeThreadTags()
{
	t_ThreadTag = TFTask::EThread::WorkerThread;

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
	const uint32_t NumTotalThreads = PlatformMisc::GetNumHardwareConcurrencyThreads(CVarUseHyperThreading.Get());

	uint32_t NumSeperateThreads = 0u;
	NumSeperateThreads += CVarUseSeperateGameThread.Get() ? 1u : 0u;
	NumSeperateThreads += CVarUseSeperateRenderThread.Get() ? 1u : 0u;
	NumSeperateThreads += CVarNumForegroundThreads.Get();

	return NumTotalThreads > NumSeperateThreads ? NumTotalThreads - NumSeperateThreads : 0u;
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

	std::scoped_lock Locker(m_Lock, Prerequisite.m_Lock);

	m_Prerequisites.insert(&Prerequisite);

	if (!IsSameExecutor(*this, Prerequisite))
	{
		Prerequisite.m_Subsequents.insert(this);
		AddRef();
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
	std::vector<TFTask*> Subsequents;

	{
		std::lock_guard<std::mutex> Locker(m_Lock);
		Subsequents.assign(m_Subsequents.begin(), m_Subsequents.end());
	}

	for (auto Subsequent : Subsequents)
	{
		if (Subsequent && Subsequent->ReleaseRef())
		{
			Subsequent->Trigger();
		}
	}
}

bool TFTask::Restart()
{
	if (!IsDispatched() || !IsCompleted())
	{
		return false;
	}

	{
		std::lock_guard<std::mutex> Locker(m_Lock);
		m_AsyncTask.reset();
		m_Future.reset();
	}

	SetCanceled(false);
	m_Completed.store(false, std::memory_order_release);
	m_Dispatched.store(false, std::memory_order_release);

	return Trigger();
}

bool TFTask::Trigger()
{
	bool Expected = false;
	if (!m_Dispatched.compare_exchange_strong(Expected, true, std::memory_order_acq_rel, std::memory_order_acquire))
	{
		return false;
	}

	if (IsCanceled() || HasAnyRef())
	{
		m_Dispatched.store(false, std::memory_order_release);
		return false;
	}

	auto Executor = TFExecutorManager::Get().GetExecutor(m_Thread, m_Priority);
	if (!Executor)
	{
		m_Dispatched.store(false, std::memory_order_release);
		return false;
	}

	std::vector<TFTask*> Prerequisites;
	{
		std::lock_guard<std::mutex> Locker(m_Lock);
		Prerequisites.assign(m_Prerequisites.begin(), m_Prerequisites.end());
	}

	std::vector<tf::AsyncTask> PrerequisiteTasks;
	PrerequisiteTasks.reserve(Prerequisites.size());

	for (auto Prerequisite : Prerequisites)
	{
		if (!Prerequisite)
		{
			continue;
		}

		Prerequisite->Trigger();

		const tf::AsyncTask PrerequisiteTask = Prerequisite->GetAsyncTask();
		if (!PrerequisiteTask.empty())
		{
			PrerequisiteTasks.emplace_back(PrerequisiteTask);
		}
	}

	std::pair<tf::AsyncTask, std::future<void>> Result = Executor->dependent_async([this]() {
		Execute();
		TriggerSubsequents();
		m_Completed.store(true, std::memory_order_release);
	}, PrerequisiteTasks.begin(), PrerequisiteTasks.end());

	{
		std::lock_guard<std::mutex> Locker(m_Lock);
		m_AsyncTask = std::move(Result.first);
		m_Future = std::make_shared<std::future<void>>(std::move(Result.second));
	}

	return true;
}

bool TFTask::Wait()
{
	std::shared_ptr<std::future<void>> Future;
	{
		std::lock_guard<std::mutex> Locker(m_Lock);
		Future = m_Future;
	}

	if (!Future)
	{
		return false;
	}

	if (!Future->valid())
	{
		return m_Completed.load(std::memory_order_acquire);
	}

	Future->get();
	return true;
}

bool TFTask::WaitForSeconds(size_t Seconds)
{
	return WaitForMilliseconds(Seconds * 1000u);
}

bool TFTask::WaitForMilliseconds(size_t Milliseconds)
{
	std::shared_ptr<std::future<void>> Future;
	{
		std::lock_guard<std::mutex> Locker(m_Lock);
		Future = m_Future;
	}

	if (!Future)
	{
		return false;
	}

	if (!Future->valid())
	{
		return m_Completed.load(std::memory_order_acquire);
	}

	return Future->wait_for(std::chrono::milliseconds(Milliseconds)) == std::future_status::ready;
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
		LOG_WARNING(LogTaskFlow, "Unexpected wait by task: {}", GetName().Get());

		try
		{
			Wait();
		}
		catch (const std::exception& Exception)
		{
			LOG_ERROR(LogTaskFlow, "Task \"{}\" threw an exception: {}", GetName().Get(), Exception.what());
		}
		catch (...)
		{
			LOG_ERROR(LogTaskFlow, "Task \"{}\" threw an unknown exception.", GetName().Get());
		}
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