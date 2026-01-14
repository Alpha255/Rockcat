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

void TTask::Launch(EThread Thread)
{
	if (!m_Launched)
	{
		m_Launched = true;

		auto Task = m_Flow.emplace([this] {
			this->Execute();
		});

		for (auto& Prerequisite : m_Prerequisites)
		{
			Task.precede(m_Flow.emplace([Prerequisite] {
				Prerequisite->Execute();
			}));
		}

		//m_Event = TaskFlow::Get().DispatchTaskFlow(m_Flow, Thread, m_Priority);
	}
}