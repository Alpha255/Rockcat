#pragma once

#include "Core/Module.h"
#include "Async/Task.h"

class TaskFlow : public IService<TaskFlow>
{
public:
	TaskFlow();

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

			tf::Future<void> Future = Executor->run(std::move(Flow));
			return std::make_shared<TaskEvent>(Future);
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

			tf::Future<void> Future = Executor->run(std::move(Flow));
			return std::make_shared<TaskEvent>(Future);
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
			std::future<void> Future = Executor->async(std::forward<Callable>(Function));
			return std::make_shared<TaskEvent>(Future);
		}

		std::forward<Callable>(Function)();
		return std::make_shared<TaskEvent>();
	}

	TaskEventPtr DispatchTask(Task& InTask, EThread Thread, Task::EPriority Priority = Task::EPriority::Normal)
	{
		assert(Thread < EThread::Num);

		if (auto Executor = GetExecutor(Thread, Priority))
		{
			std::future<void> Future = Executor->async([&InTask]() {
				InTask.Execute();
			});
			return std::make_shared<TaskEvent>(Future);
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
					}).name(Task->GetName().Get().data());
				}
			}

			tf::Future<void> Future = Executor->run(std::move(Flow));
			return std::make_shared<TaskEvent>(Future);
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

	TaskEventPtr DispatchTaskSet(TaskSet& Set, EThread Thread, Task::EPriority Priority = Task::EPriority::Normal)
	{
		assert(Thread < EThread::Num && Thread != EThread::GameThread && Thread != EThread::RenderThread);

		if (auto Executor = GetExecutor(Thread, Priority))
		{
			tf::Future<void> Future = GetExecutor(Thread, Priority)->run(Set);
			return std::make_shared<TaskEvent>(Future);
		}

		return std::make_shared<TaskEvent>();
	}

	template<class TaskBody, class PrerequisiteTasks>
	void Launch(
		FName&& Name, 
		TaskBody&& Body, 
		PrerequisiteTasks&& Prerequisites, 
		EThread Thread = EThread::WorkerThread, 
		::Task::EPriority Priority = ::Task::EPriority::Normal)
	{
	}

	template<class TaskBody, class PrerequisiteTasks>
	void Launch(
		FName&& Name,
		TaskBody&& Body,
		EThread Thread = EThread::WorkerThread,
		::Task::EPriority Priority = ::Task::EPriority::Normal)
	{
	}

	inline uint8_t GetNumWorkerThreads() const { return m_NumWorkerThreads; }

	void FrameSync(bool AllowOneFrameLag);

	tf::Executor* GetExecutor(EThread Thread, Task::EPriority Priority);
private:

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
		TaskFlow::Get().ParallelFor(Begin, End, Function, Thread, Priority)->Wait();
	}

	template<class Iterator, class Callable>
	inline TaskEventPtr ParallelFor(Iterator&& Begin, Iterator&& End, Callable&& Function, EThread Thread = EThread::WorkerThread, ::Task::EPriority Priority = ::Task::EPriority::Normal)
	{
		return TaskFlow::Get().ParallelFor(Begin, End, Function, Thread, Priority);
	}

	template<class Iterator, class CompareOp>
	inline void ParallelSort_WaitDone(Iterator&& Begin, Iterator&& End, CompareOp&& Function, EThread Thread = EThread::WorkerThread, ::Task::EPriority Priority = ::Task::EPriority::Normal)
	{
		TaskFlow::Get().ParallelSort(Begin, End, Function, Thread, Priority)->Wait();
	}

	template<class Iterator, class CompareOp>
	inline TaskEventPtr ParallelSort(Iterator&& Begin, Iterator&& End, CompareOp&& Function, EThread Thread = EThread::WorkerThread, ::Task::EPriority Priority = ::Task::EPriority::Normal)
	{
		return TaskFlow::Get().ParallelSort(Begin, End, Function, Thread, Priority);
	}

	template<class Callable>
	inline void Async_WaitDone(Callable&& Function, EThread Thread = EThread::WorkerThread, ::Task::EPriority Priority = ::Task::EPriority::Normal)
	{
		TaskFlow::Get().Async(Function, Thread, Priority)->Wait();
	}

	template<class Callable>
	inline TaskEventPtr Async(Callable&& Function, EThread Thread = EThread::WorkerThread, ::Task::EPriority Priority = ::Task::EPriority::Normal)
	{
		return TaskFlow::Get().Async(Function, Thread, Priority);
	}

	inline void DispatchTask_WaitDone(::Task& InTask, EThread Thread = EThread::WorkerThread, ::Task::EPriority Priority = ::Task::EPriority::Normal)
	{
		TaskFlow::Get().DispatchTask(InTask, Thread, Priority)->Wait();
	}

	inline TaskEventPtr DispatchTask(::Task& InTask, EThread Thread = EThread::WorkerThread, ::Task::EPriority Priority = ::Task::EPriority::Normal)
	{
		return TaskFlow::Get().DispatchTask(InTask, Thread, Priority);
	}

	inline void DispatchTasks_WaitDone(const std::vector<::Task*>& InTasks, EThread Thread = EThread::WorkerThread, ::Task::EPriority Priority = ::Task::EPriority::Normal)
	{
		TaskFlow::Get().DispatchTasks(InTasks, Thread, Priority)->Wait();
	}

	inline TaskEventPtr DispatchTasks(const std::vector<::Task*>& InTasks, EThread Thread = EThread::WorkerThread, ::Task::EPriority Priority = ::Task::EPriority::Normal)
	{
		return TaskFlow::Get().DispatchTasks(InTasks, Thread, Priority);
	}

	inline void DispatchTaskSet_WaitDone(TaskSet& Set, EThread Thread = EThread::WorkerThread, ::Task::EPriority Priority = ::Task::EPriority::Normal)
	{
		TaskFlow::Get().DispatchTaskSet(Set, Thread, Priority)->Wait();
	}

	inline TaskEventPtr DispatchTaskSet(TaskSet& Set, EThread Thread = EThread::WorkerThread, ::Task::EPriority Priority = ::Task::EPriority::Normal)
	{
		return TaskFlow::Get().DispatchTaskSet(Set, Thread, Priority);
	}
}