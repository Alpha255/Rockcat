#pragma once

#include "Core/Module.h"
#include "Engine/Async/Task.h"

#pragma warning(push)
#pragma warning(disable:4244)
#pragma warning(disable:4456)
#include <Submodules/taskflow/taskflow/taskflow.hpp>
#include <Submodules/taskflow/taskflow/algorithm/for_each.hpp>
#include <Submodules/taskflow/taskflow/algorithm/sort.hpp>
#pragma warning(pop)

enum class EThread
{
	GameThread,
	RenderThread,
	FileWatchThread,
	WorkerThread,
	ThreadTypes
};

class TaskFlowService : public IService<TaskFlowService>
{
public:
	TaskFlowService();

	void OnStartup() override final;

	void OnShutdown() override final;

	template<bool WaitDone = true, class Iterator, class Callable>
	void ParallelFor(Iterator&& Begin, Iterator&& End, Callable&& Function, EThread Thread = EThread::WorkerThread)
	{
		tf::Taskflow TFTaskflow;
		TFTaskflow.for_each(std::forward<Iterator>(Begin), std::forward<Iterator>(End), std::forward<Callable>(Function));
		if (WaitDone)
		{
			m_Executors[(size_t)Thread]->run(TFTaskflow).wait();
		}
		else
		{
			m_Executors[(size_t)Thread]->run(TFTaskflow);
		}
	}

	template<size_t Step, bool WaitDone = true, class Callable>
	void ParallelForIndex(size_t Begin, size_t End, Callable&& Function, EThread Thread = EThread::WorkerThread)
	{
		assert(Step < (End - Begin));

		tf::Taskflow TFTaskflow;
		TFTaskflow.for_each_index(Begin, End, Step, std::forward<Callable>(Function));
		if (WaitDone)
		{
			m_Executors[(size_t)Thread]->run(TFTaskflow).wait();
		}
		else
		{
			m_Executors[(size_t)Thread]->run(TFTaskflow);
		}
	}

	template<bool WaitDone = true, class Iterator, class CompareOp>
	void ParallelSort(Iterator&& Begin, Iterator&& End, CompareOp&& Function, EThread Thread = EThread::WorkerThread)
	{
		tf::Taskflow TFTaskflow;
		TFTaskflow.sort(std::forward<Iterator>(Begin), std::forward<Iterator>(End), std::forward<CompareOp>(Function));
		if (WaitDone)
		{
			m_Executors[(size_t)Thread]->run(TFTaskflow).wait();
		}
		else
		{
			m_Executors[(size_t)Thread]->run(TFTaskflow);
		}
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

	void DispatchTask(Task& InTask, EThread Thread = EThread::WorkerThread)
	{
		m_Executors[(size_t)Thread]->silent_async(std::string(InTask.GetName()), [&InTask]() {
			InTask.Execute();
		});
	}

	template<bool WaitDone = false>
	void DispatchTasks(std::vector<Task&>& InTasks, EThread Thread = EThread::WorkerThread)
	{
		tf::Taskflow TempTaskflow;
		for each (auto& Task in InTasks)
		{
			tf::Task TFTask = TempTaskflow.emplace([&Task]() {
				Task.Execute();
			});
			TFTask.name(std::string(Task.GetName()))
				.priority(static_cast<tf::TaskPriority>(Task.GetPriority()));
		}
		if (WaitDone)
		{
			m_Executors[(size_t)Thread]->run(TempTaskflow).wait();
		}
		else
		{
			m_Executors[(size_t)Thread]->run(TempTaskflow);
		}
	}
private:
	bool m_UseHyperThreading;
	std::array<std::unique_ptr<tf::Executor>, (size_t)EThread::ThreadTypes> m_Executors;
};