#pragma once

#include "Core/Module.h"
#include "Core/PlatformMisc.h"

enum class EThread
{
	GameThread,
	RenderThread,
	FileWatchThread,
	WorkerThread,
	Num
};

class TaskFlowService : public IService<TaskFlowService>
{
public:
	TaskFlowService();

	void OnStartup() override final;

	void OnShutdown() override final;

	template<bool WaitDone = true, class Iterator, class Callable>
	tf::Future<void> ParallelFor(Iterator&& Begin, Iterator&& End, Callable&& Function, EThread Thread = EThread::WorkerThread)
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
	tf::Future<void> ParallelForRange(size_t Begin, size_t End, Callable&& Function, EThread Thread = EThread::WorkerThread)
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
	tf::Future<void> ParallelSort(Iterator&& Begin, Iterator&& End, CompareOp&& Function, EThread Thread = EThread::WorkerThread)
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
	void Async(Callable&& Function, EThread Thread = EThread::WorkerThread)
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
	tf::Future<void> DispatchTask(Task& InTask, EThread Thread = EThread::WorkerThread)
	{
		tf::Taskflow Taskflow;
		Taskflow.emplace([&InTask]() {
			PlatformMisc::ThreadPriorityGuard PriorityGuard(std::this_thread::get_id(), InTask.GetPriority());
			InTask.Execute();
		}).name(std::string(InTask.GetName()));
		
		auto Future = m_Executors[(size_t)Thread]->run(Taskflow);
		if (WaitDone)
		{
			Future.wait();
		}
		return Future;
	}

	template<bool WaitDone = false>
	tf::Future<void> DispatchTasks(std::vector<Task*>& InTasks, EThread Thread = EThread::WorkerThread)
	{
		tf::Taskflow Taskflow;
		for (auto& Task : InTasks)
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

	template<bool WaitDone = false>
	tf::Future<void> ExecuteTaskFlow(TaskFlow& Taskflow, EThread Thread = EThread::WorkerThread)
	{
		auto Future = m_Executors[(size_t)Thread]->run(std::move(Taskflow));
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
	std::array<std::unique_ptr<tf::Executor>, (size_t)EThread::Num> m_Executors;
};