#include "Async/Task.h"
#include "Services/TaskFlowService.h"

thread_local EThread t_ThreadTag = EThread::WorkerThread;

void TaskFlowTask::InitializeThreadTags()
{
	t_ThreadTag = EThread::MainThread;

	tf::Async_WaitDone([]() {
		t_ThreadTag = EThread::GameThread;
	}, EThread::GameThread);

	tf::Async_WaitDone([]() {
		t_ThreadTag = EThread::RenderThread;
	}, EThread::RenderThread);
}

bool Task::IsMainThread()
{
	return t_ThreadTag == EThread::MainThread;
}

bool Task::IsGameThread()
{
	return t_ThreadTag == EThread::GameThread;
}

bool Task::IsRenderThread()
{
	return t_ThreadTag == EThread::RenderThread;
}

bool Task::IsWorkerThread()
{
	return t_ThreadTag == EThread::WorkerThread;
}

void Task::Dispatch(EThread Thread)
{
	if (!m_Event || m_Event->IsCompleted())
	{
		m_Event = TaskFlow::Get().DispatchTask(*this, Thread, m_Priority);
	}
}

void Task::Execute()
{
	if (!m_Executing)
	{
		m_Executing = true;
		ExecuteImpl();
		m_Executing = false;
	}
}

void TaskSet::Dispatch(EThread Thread)
{
	if (!m_Event || m_Event->IsCompleted())
	{
		m_Event = TaskFlow::Get().DispatchTaskSet(*this, Thread, m_Priority);
	}
}

bool TFTask::AddPrerequisite(TFTask& Prerequisite)
{
	if (!Prerequisite.AddSubsequents(*this))
	{
		return false;
	}

	std::lock_guard<std::mutex> Locker(m_Lock);
	m_Prerequisites.insert(&Prerequisite);

	return true;
}

bool TFTask::AddSubsequents(TFTask& Subsequent)
{
	std::lock_guard<std::mutex> Locker(m_Lock);
	m_Subsequents.insert(&Subsequent);
}

void TFTask::Execute()
{
	if (m_TaskFunc)
	{
		m_TaskFunc();
	}
}

void TFTask::Launch()
{
	if (IsCanceled())
	{
		return;
	}

	if (!IsDispatched())
	{
		if (auto Executor = TaskFlow::Get().GetExecutor(m_Thread, m_Priority))
		{
			std::vector<tf::AsyncTask> PrerequisiteTasks;
			PrerequisiteTasks.reserve(m_Prerequisites.size());

			for (auto Prerequisite : m_Prerequisites)
			{
				if (Prerequisite)
				{
					Prerequisite->Launch();

					if (auto LowLevelTask = Prerequisite->GetLowLevelTask())
					{
						PrerequisiteTasks.emplace_back(tf::AsyncTask(*LowLevelTask));
					}
				}
			}

			m_LowLevelTask = std::make_shared<LowLevelTask>(std::move(Executor->dependent_async([this]() {
				Execute();
			}, PrerequisiteTasks.begin(), PrerequisiteTasks.end())));
		}
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
