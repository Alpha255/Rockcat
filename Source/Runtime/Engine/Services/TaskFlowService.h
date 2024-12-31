#pragma once

#include "Core/Module.h"
#include "Core/PlatformMisc.h"
#include "Engine/Async/TaskFlow.h"

class TaskFlowService : public IService<TaskFlowService>
{
public:
	TaskFlowService();

	void OnStartup() override final;

	void OnShutdown() override final;

	template<bool WaitDone = true, class Iterator, class Callable>
	const TaskEvent& ParallelFor(Iterator&& Begin, Iterator&& End, Callable&& Function, EThread Thread)
	{
		auto& Flow = CreateTaskFlow();
		Flow.for_each(std::forward<Iterator>(Begin), std::forward<Iterator>(End), std::forward<Callable>(Function));
		Flow.Execute(*m_Executors[(size_t)Thread]);

		if (WaitDone)
		{
			Flow.Wait();
		}
		return Flow.GetEvent();
	}

	template<size_t Step, bool WaitDone = true, class Callable>
	const TaskEvent& ParallelForRange(size_t Begin, size_t End, Callable&& Function, EThread Thread)
	{
		assert(Step < (End - Begin));

		auto& Flow = CreateTaskFlow();
		Flow.for_each_index(Begin, End, Step, std::forward<Callable>(Function));
		Flow.Execute(*m_Executors[(size_t)Thread]);

		if (WaitDone)
		{
			Flow.Wait();
		}
		return Flow.GetEvent();
	}

	template<bool WaitDone = true, class Iterator, class CompareOp>
	const TaskEvent& ParallelSort(Iterator&& Begin, Iterator&& End, CompareOp&& Function, EThread Thread)
	{
		auto& Flow = CreateTaskFlow();
		Flow.sort(std::forward<Iterator>(Begin), std::forward<Iterator>(End), std::forward<CompareOp>(Function));
		Flow.Execute(*m_Executors[(size_t)Thread]);

		if (WaitDone)
		{
			Flow.Wait();
		}
		return Flow.GetEvent();
	}

	template<bool WaitDone = false, class Callable>
	void Async(Callable&& Function, EThread Thread)
	{
		if (WaitDone)
		{
			m_Executors[(size_t)Thread]->async(std::forward<Callable>(Function)).wait();
		}
		else
		{
			m_Executors[(size_t)Thread]->silent_async(std::forward<Callable>(Function));
		}
	}

	template<bool WaitDone = false>
	const TaskEvent& DispatchTask(Task& InTask, EThread Thread)
	{
		auto& Flow = CreateTaskFlow();
		Flow.emplace([&InTask]() {
			PlatformMisc::ThreadPriorityGuard PriorityGuard(std::this_thread::get_id(), InTask.GetPriority());
			InTask.Execute();
		}).name(std::string(InTask.GetName()));
		
		Flow.Execute(*m_Executors[(size_t)Thread]);
		if (WaitDone)
		{
			Flow.Wait();
		}
		return Flow.GetEvent();
	}

	template<bool WaitDone = false>
	const TaskEvent& DispatchTasks(const std::vector<Task*>& InTasks, EThread Thread)
	{
		auto& Flow = CreateTaskFlow();
		for (auto& Task : InTasks)
		{
			if (Task)
			{
				Flow.emplace([&Task]() {
					Task->Execute();
				}).name(std::string(Task->GetName()));
			}
		}

		Flow.Execute(*m_Executors[(size_t)Thread]);

		if (WaitDone)
		{
			Flow.Wait();
		}
		return Flow.GetEvent();
	}

	template<bool WaitDone = false>
	const TaskEvent& DispatchTaskFlow(TaskFlow& Flow, EThread Thread)
	{
		Flow.Execute(*m_Executors[(size_t)Thread]);

		if (WaitDone)
		{
			Flow.Wait();
		}
		return Flow.GetEvent();
	}

	uint8_t GetNumWorkThreads() const { return m_NumWorkThreads; }
private:
	TaskFlow& CreateTaskFlow() { return m_TaskFlows.emplace_back(TaskFlow()); }

	void UpdateTaskFlows();

	bool m_UseHyperThreading;
	uint8_t m_NumWorkThreads;
	std::list<TaskFlow> m_TaskFlows;
	std::array<std::unique_ptr<tf::Executor>, (size_t)EThread::Num> m_Executors;
};

namespace tf
{
	template<class Iterator, class Callable>
	inline const TaskEvent& ParallelFor_WaitDone(Iterator&& Begin, Iterator&& End, Callable&& Function, EThread Thread = EThread::WorkerThread)
	{
		return TaskFlowService::Get().ParallelFor<true>(Begin, End, Function, Thread);
	}

	template<class Iterator, class Callable>
	inline const TaskEvent& ParallelFor(Iterator&& Begin, Iterator&& End, Callable&& Function, EThread Thread = EThread::WorkerThread)
	{
		return TaskFlowService::Get().ParallelFor<false>(Begin, End, Function, Thread);
	}

	template<size_t Step, class Callable>
	inline const TaskEvent& ParallelForRange_WaitDone(size_t Begin, size_t End, Callable&& Function, EThread Thread = EThread::WorkerThread)
	{
		return TaskFlowService::Get().ParallelForRange<Step, true>(Begin, End, Function, Thread);
	}

	template<size_t Step, class Callable>
	inline const TaskEvent& ParallelForRange(size_t Begin, size_t End, Callable&& Function, EThread Thread = EThread::WorkerThread)
	{
		return TaskFlowService::Get().ParallelForRange<Step, false>(Begin, End, Function, Thread);
	}

	template<class Iterator, class CompareOp>
	inline const TaskEvent& ParallelSort_WaitDone(Iterator&& Begin, Iterator&& End, CompareOp&& Function, EThread Thread = EThread::WorkerThread)
	{
		return TaskFlowService::Get().ParallelSort<true>(Begin, End, Function, Thread);
	}

	template<class Iterator, class CompareOp>
	inline const TaskEvent& ParallelSort(Iterator&& Begin, Iterator&& End, CompareOp&& Function, EThread Thread = EThread::WorkerThread)
	{
		return TaskFlowService::Get().ParallelSort<false>(Begin, End, Function, Thread);
	}

	template<class Callable>
	inline void Async_WaitDone(Callable&& Function, EThread Thread = EThread::WorkerThread)
	{
		return TaskFlowService::Get().Async<true>(Function, Thread);
	}

	template<class Callable>
	inline void Async(Callable&& Function, EThread Thread = EThread::WorkerThread)
	{
		return TaskFlowService::Get().Async<false>(Function, Thread);
	}

	inline const TaskEvent& DispatchTask_WaitDone(::Task& InTask, EThread Thread = EThread::WorkerThread)
	{
		return TaskFlowService::Get().DispatchTask<true>(InTask, Thread);
	}

	inline const TaskEvent& DispatchTask(::Task& InTask, EThread Thread = EThread::WorkerThread)
	{
		return TaskFlowService::Get().DispatchTask<false>(InTask, Thread);
	}

	inline const TaskEvent& DispatchTasks_WaitDone(const std::vector<::Task*>& InTasks, EThread Thread = EThread::WorkerThread)
	{
		return TaskFlowService::Get().DispatchTasks<true>(InTasks, Thread);
	}

	inline const TaskEvent& DispatchTasks(const std::vector<::Task*>& InTasks, EThread Thread = EThread::WorkerThread)
	{
		return TaskFlowService::Get().DispatchTasks<false>(InTasks, Thread);
	}

	inline const TaskEvent& DispatchTaskFlow_WaitDone(TaskFlow& Flow, EThread Thread = EThread::WorkerThread)
	{
		return TaskFlowService::Get().DispatchTaskFlow<true>(Flow, Thread);
	}

	inline const TaskEvent& DispatchTaskFlow(TaskFlow& Flow, EThread Thread = EThread::WorkerThread)
	{
		return TaskFlowService::Get().DispatchTaskFlow<false>(Flow, Thread);
	}
}