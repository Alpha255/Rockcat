#include "Async/Task.h"
#include "System/System.h"
#include "Services/TaskFlowService.h"

thread_local TFTask::EThread t_ThreadTag = TFTask::EThread::WorkerThread;

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

void TFTask::InitializeThreadTags()
{
	t_ThreadTag = TFTask::EThread::MainThread;

	tf::Async_WaitDone([]() {
		t_ThreadTag = TFTask::EThread::GameThread;
	}, TFTask::EThread::GameThread);

	tf::Async_WaitDone([]() {
		t_ThreadTag = TFTask::EThread::RenderThread;
	}, EThread::RenderThread);
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
	if (IsCanceled() || IsDispatched())
	{
		return;
	}

	if (auto Executor = TaskFlow::Get().GetExecutor(m_Thread, m_Priority))
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

		m_AsyncTask = std::make_shared<tf::AsyncTask>(std::move(Executor->dependent_async([this]() {
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
