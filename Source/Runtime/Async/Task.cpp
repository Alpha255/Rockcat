#include "Async/Task.h"
#include "Async/TaskExecutorManager.h"
#include "Application/Application.h"

DEFINE_LOGGER_CATEGORY(LogTaskFlow);

struct SetThreadPriorityScoped
{
	SetThreadPriorityScoped(ETFTaskPriority Priority)
		: SkipPriorityChange(!TFTask::IsWorkerThread() || Priority == ETFTaskPriority::Normal)
	{
		if (!SkipPriorityChange)
		{
			Application::SetThreadPriority(std::this_thread::get_id(), Priority);
		}
	}

	~SetThreadPriorityScoped()
	{
		if (!SkipPriorityChange)
		{
			Application::SetThreadPriority(std::this_thread::get_id(), ETFTaskPriority::Normal);
		}
	}

	bool SkipPriorityChange;
};

TFTaskEventPtr TFTask::DispatchTaskFlow(tf::Taskflow&& Flow, ETFTaskThread Thread, ETFTaskPriority Priority)
{
	if (auto Executor = TFTaskExecutorManager::Get().GetExecutor(Thread, Priority))
	{
		return std::make_shared<TFTaskEvent>(std::move(Executor->run(std::forward<tf::Taskflow>(Flow))));
	}

	return nullptr;
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

	auto Executor = TFTaskExecutorManager::Get().GetExecutor(m_Thread, m_Priority);
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

		// Prerequisites running on another executor are already enforced by the reference
		// counting in AddPrerequisite()/TriggerSubsequents(): this task is not dispatched until
		// every one of them released its reference, so they have all finished by now.
		// Their tf::AsyncTask must NOT be handed to this executor either, because
		// tf::Executor::_process_dependent_async() pushes our node into the predecessor node's
		// edge list, which makes the *other* executor schedule and tear down a node that belongs
		// to this one.
		if (!IsSameExecutor(*this, *Prerequisite))
		{
			continue;
		}

		const tf::AsyncTask PrerequisiteTask = Prerequisite->GetAsyncTask();
		if (!PrerequisiteTask.empty())
		{
			PrerequisiteTasks.emplace_back(PrerequisiteTask);
		}
	}

	std::pair<tf::AsyncTask, std::future<void>> Result = Executor->dependent_async([this]() {
		Execute();

		// Publish the completion as soon as our own work is done. TriggerSubsequents() runs
		// afterwards and can dispatch (and wait on) downstream tasks, which would otherwise keep
		// this task reporting "not completed" long after it actually finished.
		m_Completed.store(true, std::memory_order_release);

		TriggerSubsequents();
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
//		m_ThreadSyncEvents[m_ThreadEventIndex] = DispatchTask(s_FrameSyncTask, ETFTaskThread::RenderThread);
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
//				auto GameThreadCompledEvent = DispatchTask(s_FrameSyncTask, ETFTaskThread::GameThread);
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