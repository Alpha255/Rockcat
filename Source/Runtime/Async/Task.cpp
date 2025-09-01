#include "Async/Task.h"
#include "Services/TaskFlowService.h"

thread_local EThread t_ThreadTag = EThread::WorkerThread;

void ITask::InitializeThreadTags()
{
	t_ThreadTag = EThread::MainThread;

	tf::Async_WaitDone([]() {
		t_ThreadTag = EThread::GameThread;
		}, EThread::GameThread);

	tf::Async_WaitDone([]() {
		t_ThreadTag = EThread::RenderThread;
		}, EThread::RenderThread);
}

bool ITask::IsInMainThread()
{
	return t_ThreadTag == EThread::MainThread;
}

bool ITask::IsInGameThread()
{
	return t_ThreadTag == EThread::GameThread;
}

bool ITask::IsInRenderThread()
{
	return t_ThreadTag == EThread::RenderThread;
}

bool ITask::IsInWorkerThread()
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