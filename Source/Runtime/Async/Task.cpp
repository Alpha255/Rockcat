#include "Async/Task.h"
#include "Services/TaskFlowService.h"

thread_local EThread t_ThreadTag = EThread::WorkerThread;

void Task::Dispatch(EThread Thread)
{
	if (!m_Event || m_Event->IsCompleted())
	{
		m_Event = TaskFlowService::Get().DispatchTask(*this, Thread, m_Priority);
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

void Task::InitializeThreadTags()
{
	t_ThreadTag = EThread::MainThread;

	tf::Async_WaitDone([]() {
		t_ThreadTag = EThread::GameThread;
		}, EThread::GameThread);

	tf::Async_WaitDone([]() {
		t_ThreadTag = EThread::RenderThread;
		}, EThread::RenderThread);
}

bool Task::IsInMainThread()
{
	return t_ThreadTag == EThread::MainThread;
}

bool Task::IsInGameThread()
{
	return t_ThreadTag == EThread::GameThread;
}

bool Task::IsInRenderThread()
{
	return t_ThreadTag == EThread::RenderThread;
}

bool Task::IsInWorkerThread()
{
	return t_ThreadTag == EThread::WorkerThread;
}