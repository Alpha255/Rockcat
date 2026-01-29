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

void TTask::Launch()
{
	if (!IsDispatched())
	{
		tf::Executor* Executor = nullptr;
		m_LowLevelTask = std::make_shared(std::move(Executor->dependent_async([this]() {
			Execute();
		})));
	}
}

bool TTask::TryLaunch()
{
	for (auto Prerequisite : m_Prerequisites)
	{
		if (!Prerequisite->IsCompleted())
		{
			return false;
		}
	}

	return false;
}

bool TTask::TryCancel()
{
	return false;
}
