#pragma once

#include "Core/Module.h"
#include "Async/TaskFlow.h"

class TaskFlowService : public IService<TaskFlowService>
{
public:
	TaskFlowService();

	void Initialize() override final;
	void Finalize() override final;

	template<class Iterator, class Callable>
	TaskEventPtr ParallelFor(Iterator&& Begin, Iterator&& End, Callable&& Function, EThread Thread, Task::EPriority Priority = Task::EPriority::Normal)
	{
		assert(Thread < EThread::Num);

		if (auto Executor = GetExecutor(Thread, Priority))
		{
			tf::Taskflow Flow;
			Flow.for_each(std::forward<Iterator>(Begin), std::forward<Iterator>(End), std::forward<Callable>(Function));

			return std::make_shared<TaskEvent>(std::move(Executor->run(std::move(Flow))));
		}

		std::for_each(std::forward<Iterator>(Begin), std::forward<Iterator>(End), std::forward<Callable>(Function));
		return std::make_shared<TaskEvent>();
	}

	template<class Iterator, class CompareOp>
	TaskEventPtr ParallelSort(Iterator&& Begin, Iterator&& End, CompareOp&& Function, EThread Thread, Task::EPriority Priority = Task::EPriority::Normal)
	{
		assert(Thread < EThread::Num);

		if (auto Executor = GetExecutor(Thread, Priority))
		{
			tf::Taskflow Flow;
			Flow.sort(std::forward<Iterator>(Begin), std::forward<Iterator>(End), std::forward<CompareOp>(Function));
			return std::make_shared<TaskEvent>(std::move(Executor->run(std::move(Flow))));
		}

		std::sort(std::forward<Iterator>(Begin), std::forward<Iterator>(End), std::forward<CompareOp>(Function));
		return std::make_shared<TaskEvent>();
	}

	template<class Callable>
	TaskEventPtr Async(Callable&& Function, EThread Thread, Task::EPriority Priority = Task::EPriority::Normal)
	{
		assert(Thread < EThread::Num);

		if (auto Executor = GetExecutor(Thread, Priority))
		{
			return std::make_shared<TaskEvent>(std::move(Executor->async(std::forward<Callable>(Function))));
		}

		std::forward<Callable>(Function)();
		return std::make_shared<TaskEvent>();
	}

	TaskEventPtr DispatchTask(Task& InTask, EThread Thread, Task::EPriority Priority = Task::EPriority::Normal)
	{
		assert(Thread < EThread::Num);

		if (auto Executor = GetExecutor(Thread, Priority))
		{
			return std::make_shared<TaskEvent>(std::move(Executor->async([&InTask]() {
				InTask.Execute();
				})));
		}

		InTask.Execute();
		return std::make_shared<TaskEvent>();
	}

	TaskEventPtr DispatchTasks(const std::vector<Task*>& InTasks, EThread Thread, Task::EPriority Priority = Task::EPriority::Normal)
	{
		assert(Thread < EThread::Num);

		if (auto Executor = GetExecutor(Thread, Priority))
		{
			tf::Taskflow Flow;
			for (auto& Task : InTasks)
			{
				if (Task)
				{
					Flow.emplace([&Task]() {
						Task->Execute();
						}).name(std::string(Task->GetName()));
				}
			}

			return std::make_shared<TaskEvent>(std::move(Executor->run(std::move(Flow))));
		}

		for (auto& Task : InTasks)
		{
			if (Task)
			{
				Task->Execute();
			}
		}
		return std::make_shared<TaskEvent>();
	}

	TaskEventPtr DispatchTaskFlow(TaskFlow& Flow, EThread Thread, Task::EPriority Priority = Task::EPriority::Normal)
	{
		assert(Thread < EThread::Num && Thread != EThread::GameThread && Thread != EThread::RenderThread);

		Flow.Dispatch(*GetExecutor(Thread, Priority));
		return Flow.GetEvent();
	}

	inline uint8_t GetNumWorkerThreads() const { return m_NumWorkerThreads; }

	void FrameSync(bool AllowOneFrameLag);
private:
	tf::Executor* GetExecutor(EThread Thread, Task::EPriority Priority);

	bool m_UseHyperThreading;
	bool m_SeparateGameThread = false;
	bool m_SeparateRenderThread = false;
	uint8_t m_NumThreads;
	uint8_t m_NumWorkerThreads;
	uint8_t m_NumSeperateThreads;
	uint8_t m_NumForegroundThreads;
	std::vector<std::unique_ptr<tf::Executor>> m_Executors;
	std::array<TaskEventPtr, 2u> m_ThreadSyncEvents;
	uint32_t m_ThreadEventIndex = 0u;
};

namespace tf
{
	template<class Iterator, class Callable>
	inline void ParallelFor_WaitDone(Iterator&& Begin, Iterator&& End, Callable&& Function, EThread Thread = EThread::WorkerThread, ::Task::EPriority Priority = ::Task::EPriority::Normal)
	{
		TaskFlowService::Get().ParallelFor(Begin, End, Function, Thread, Priority)->Wait();
	}

	template<class Iterator, class Callable>
	inline TaskEventPtr ParallelFor(Iterator&& Begin, Iterator&& End, Callable&& Function, EThread Thread = EThread::WorkerThread, ::Task::EPriority Priority = ::Task::EPriority::Normal)
	{
		return TaskFlowService::Get().ParallelFor(Begin, End, Function, Thread, Priority);
	}

	template<class Iterator, class CompareOp>
	inline void ParallelSort_WaitDone(Iterator&& Begin, Iterator&& End, CompareOp&& Function, EThread Thread = EThread::WorkerThread, ::Task::EPriority Priority = ::Task::EPriority::Normal)
	{
		TaskFlowService::Get().ParallelSort(Begin, End, Function, Thread, Priority)->Wait();
	}

	template<class Iterator, class CompareOp>
	inline TaskEventPtr ParallelSort(Iterator&& Begin, Iterator&& End, CompareOp&& Function, EThread Thread = EThread::WorkerThread, ::Task::EPriority Priority = ::Task::EPriority::Normal)
	{
		return TaskFlowService::Get().ParallelSort(Begin, End, Function, Thread, Priority);
	}

	template<class Callable>
	inline void Async_WaitDone(Callable&& Function, EThread Thread = EThread::WorkerThread, ::Task::EPriority Priority = ::Task::EPriority::Normal)
	{
		TaskFlowService::Get().Async(Function, Thread, Priority)->Wait();
	}

	template<class Callable>
	inline TaskEventPtr Async(Callable&& Function, EThread Thread = EThread::WorkerThread, ::Task::EPriority Priority = ::Task::EPriority::Normal)
	{
		return TaskFlowService::Get().Async(Function, Thread, Priority);
	}

	inline void DispatchTask_WaitDone(::Task& InTask, EThread Thread = EThread::WorkerThread, ::Task::EPriority Priority = ::Task::EPriority::Normal)
	{
		TaskFlowService::Get().DispatchTask(InTask, Thread, Priority)->Wait();
	}

	inline TaskEventPtr DispatchTask(::Task& InTask, EThread Thread = EThread::WorkerThread, ::Task::EPriority Priority = ::Task::EPriority::Normal)
	{
		return TaskFlowService::Get().DispatchTask(InTask, Thread, Priority);
	}

	inline void DispatchTasks_WaitDone(const std::vector<::Task*>& InTasks, EThread Thread = EThread::WorkerThread, ::Task::EPriority Priority = ::Task::EPriority::Normal)
	{
		TaskFlowService::Get().DispatchTasks(InTasks, Thread, Priority)->Wait();
	}

	inline TaskEventPtr DispatchTasks(const std::vector<::Task*>& InTasks, EThread Thread = EThread::WorkerThread, ::Task::EPriority Priority = ::Task::EPriority::Normal)
	{
		return TaskFlowService::Get().DispatchTasks(InTasks, Thread, Priority);
	}

	inline void DispatchTaskFlow_WaitDone(TaskFlow& Flow, EThread Thread = EThread::WorkerThread, ::Task::EPriority Priority = ::Task::EPriority::Normal)
	{
		TaskFlowService::Get().DispatchTaskFlow(Flow, Thread, Priority)->Wait();
	}

	inline TaskEventPtr DispatchTaskFlow(TaskFlow& Flow, EThread Thread = EThread::WorkerThread, ::Task::EPriority Priority = ::Task::EPriority::Normal)
	{
		return TaskFlowService::Get().DispatchTaskFlow(Flow, Thread, Priority);
	}
}