#include "Engine/Threading/Threading.h"
#include "Engine/Services/TaskFlowService.h"

namespace Threading
{
	thread_local EThread t_ThreadTag = EThread::WorkerThread;

	void Initialize()
	{
		tf::Async_WaitDone([]() {
			t_ThreadTag = EThread::GameThread;
		}, EThread::GameThread);

		tf::Async_WaitDone([]() {
			t_ThreadTag = EThread::RenderThread;
		}, EThread::RenderThread);
	}

	bool IsInMainThread()
	{
		return false;
	}

	bool IsInGameThread()
	{
		return t_ThreadTag == EThread::GameThread;
	}

	bool IsInRenderThread()
	{
		return t_ThreadTag == EThread::RenderThread;
	}

	bool IsInWorkerThread()
	{
		return t_ThreadTag == EThread::WorkerThread;
	}
}
