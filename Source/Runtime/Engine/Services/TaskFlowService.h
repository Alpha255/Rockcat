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
	tf::Future<void> ParallelFor(Iterator&& Begin, Iterator&& End, Callable&& Function, tf::EThread Thread = tf::EThread::WorkerThread)
	{
		tf::Taskflow TaskFlow;
		TaskFlow.for_each(std::forward<Iterator>(Begin), std::forward<Iterator>(End), std::forward<Callable>(Function));
		auto Future = m_Executors[(size_t)Thread]->run(TaskFlow);
		if (WaitDone)
		{
			Future.wait();
		}
		return Future;
	}

	template<size_t Step, bool WaitDone = true, class Callable>
	tf::Future<void> ParallelForRange(size_t Begin, size_t End, Callable&& Function, tf::EThread Thread = tf::EThread::WorkerThread)
	{
		assert(Step < (End - Begin));

		tf::Taskflow TaskFlow;
		TaskFlow.for_each_index(Begin, End, Step, std::forward<Callable>(Function));
		auto Future = m_Executors[(size_t)Thread]->run(TaskFlow);

		if (WaitDone)
		{
			Future.wait();
		}
		return Future;
	}

	template<bool WaitDone = true, class Iterator, class CompareOp>
	tf::Future<void> ParallelSort(Iterator&& Begin, Iterator&& End, CompareOp&& Function, tf::EThread Thread = tf::EThread::WorkerThread)
	{
		tf::Taskflow TaskFlow;
		TaskFlow.sort(std::forward<Iterator>(Begin), std::forward<Iterator>(End), std::forward<CompareOp>(Function));
		auto Future = m_Executors[(size_t)Thread]->run(TaskFlow);

		if (WaitDone)
		{
			Future.wait();
		}
		return Future;
	}

	template<bool WaitDone = false, class Callable>
	void Async(Callable&& Function, tf::EThread Thread = tf::EThread::WorkerThread)
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
	tf::Future<void> DispatchTask(tf::ThreadTask& Task, tf::EThread Thread = tf::EThread::WorkerThread)
	{
		tf::Taskflow Taskflow;
		Taskflow.emplace([&Task]() {
			PlatformMisc::ThreadPriorityGuard PriorityGuard(std::this_thread::get_id(), Task.GetPriority());
			Task.Execute();
		}).name(std::string(Task.GetName()));
		
		auto Future = m_Executors[(size_t)Thread]->run(Taskflow);
		if (WaitDone)
		{
			Future.wait();
		}
		return Future;
	}

	template<bool WaitDone = false>
	tf::Future<void> DispatchTasks(std::vector<tf::ThreadTask*>& Tasks, tf::EThread Thread = tf::EThread::WorkerThread)
	{
		tf::Taskflow Taskflow;
		for (auto& Task : Tasks)
		{
			if (Task)
			{
				Taskflow.emplace([&Task]() {
					PlatformMisc::ThreadPriorityGuard PriorityGuard(std::this_thread::get_id(), Task->GetPriority());
					Task->Execute();
				}).name(std::string(Task->GetName()));
			}
		}

		auto Future = m_Executors[(size_t)Thread]->run(Taskflow);

		if (WaitDone)
		{
			Future.wait();
		}
		return Future;
	}

	template<class TTask, bool WaitDone = false>
	tf::Future<void> ExecuteTaskFlow(tf::ThreadTaskFlow<TTask>& TaskFlow, tf::EThread Thread = tf::EThread::WorkerThread)
	{
		auto Future = m_Executors[(size_t)Thread]->run(std::move(TaskFlow));
		if (WaitDone)
		{
			Future.wait();
		}
		return Future;
	}

	uint8_t GetNumWorkThreads() const { return m_NumWorkThreads; }
private:
	bool m_UseHyperThreading;
	uint8_t m_NumWorkThreads;
	std::array<std::unique_ptr<tf::Executor>, (size_t)tf::EThread::Num> m_Executors;
};

namespace tf
{
	template<class Iterator, class Callable>
	inline Future<void> ParallelFor_WaitDone(Iterator&& Begin, Iterator&& End, Callable&& Function, tf::EThread Thread = tf::EThread::WorkerThread)
	{
		return TaskFlowService::Get().ParallelFor<true>(Begin, End, Function, Thread);
	}

	template<class Iterator, class Callable>
	inline Future<void> ParallelFor(Iterator&& Begin, Iterator&& End, Callable&& Function, tf::EThread Thread = tf::EThread::WorkerThread)
	{
		return TaskFlowService::Get().ParallelFor<false>(Begin, End, Function, Thread);
	}

	template<size_t Step, class Callable>
	inline Future<void> ParallelForRange_WaitDone(size_t Begin, size_t End, Callable&& Function, tf::EThread Thread = tf::EThread::WorkerThread)
	{
		return TaskFlowService::Get().ParallelForRange<Step, true>(Begin, End, Function, Thread);
	}

	template<size_t Step, class Callable>
	inline Future<void> ParallelForRange(size_t Begin, size_t End, Callable&& Function, tf::EThread Thread = tf::EThread::WorkerThread)
	{
		return TaskFlowService::Get().ParallelForRange<Step, false>(Begin, End, Function, Thread);
	}

	template<class Iterator, class CompareOp>
	inline Future<void> ParallelSort_WaitDone(Iterator&& Begin, Iterator&& End, CompareOp&& Function, tf::EThread Thread = tf::EThread::WorkerThread)
	{
		return TaskFlowService::Get().ParallelSort<true>(Begin, End, Function, Thread);
	}

	template<class Iterator, class CompareOp>
	inline Future<void> ParallelSort(Iterator&& Begin, Iterator&& End, CompareOp&& Function, tf::EThread Thread = tf::EThread::WorkerThread)
	{
		return TaskFlowService::Get().ParallelSort<false>(Begin, End, Function, Thread);
	}

	template<class Callable>
	inline void Async_WaitDone(Callable&& Function, tf::EThread Thread = tf::EThread::WorkerThread)
	{
		return TaskFlowService::Get().Async<true>(Function, Thread);
	}

	template<class Callable>
	inline void Async(Callable&& Function, tf::EThread Thread = tf::EThread::WorkerThread)
	{
		return TaskFlowService::Get().Async<false>(Function, Thread);
	}

	inline Future<void> DispatchTask_WaitDone(ThreadTask& Task, tf::EThread Thread = tf::EThread::WorkerThread)
	{
		return TaskFlowService::Get().DispatchTask<true>(Task, Thread);
	}

	inline Future<void> DispatchTask(ThreadTask& Task, tf::EThread Thread = tf::EThread::WorkerThread)
	{
		return TaskFlowService::Get().DispatchTask<false>(Task, Thread);
	}

	inline Future<void> DispatchTasks_WaitDone(std::vector<ThreadTask*>& Tasks, tf::EThread Thread = tf::EThread::WorkerThread)
	{
		return TaskFlowService::Get().DispatchTasks<true>(Tasks, Thread);
	}

	inline Future<void> DispatchTasks(std::vector<ThreadTask*>& Tasks, tf::EThread Thread = tf::EThread::WorkerThread)
	{
		return TaskFlowService::Get().DispatchTasks<false>(Tasks, Thread);
	}

	template<class TTask>
	inline Future<void> ExecuteTaskFlow_WaitDone(ThreadTaskFlow<TTask>& Taskflow, tf::EThread Thread = tf::EThread::WorkerThread)
	{
		return TaskFlowService::Get().ExecuteTaskFlow<TTask, true>(Taskflow, Thread);
	}

	template<class TTask>
	inline Future<void> ExecuteTaskFlow(ThreadTaskFlow<TTask>& Taskflow, tf::EThread Thread = tf::EThread::WorkerThread)
	{
		return TaskFlowService::Get().ExecuteTaskFlow<TTask, false>(Taskflow, Thread);
	}
}