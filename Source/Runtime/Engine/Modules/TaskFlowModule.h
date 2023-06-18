#pragma once

#include "Runtime/Core/Module.h"
#include "Runtime/Engine/Async/Task.h"

#pragma warning(disable:4456)
#include <Submodules/taskflow/taskflow/taskflow.hpp>
#pragma warning(default:4456)

class TaskFlowModule : public IModule
{
public:
	TaskFlowModule();

	~TaskFlowModule() {}

	template<class Iterator, class Callable, bool8_t WaitDone = true>
	void ParallelFor(Iterator&& Begin, Iterator&& End, Callable&& Function)
	{
		tf::Taskflow TempTaskflow;
		TempTaskflow.for_each(std::forward<Iterator>(Begin), std::forward<Iterator>(End), std::forward<Callable>(Function));
		if (WaitDone)
		{
			m_Executor->run(TempTaskflow).wait();
		}
		else
		{
			m_Executor->run(TempTaskflow);
		}
	}

	template<class Iterator, class Callable, size_t Step, bool8_t WaitDone = true>
	void ParallelFor(Iterator&& Begin, Iterator&& End, Callable&& Function)
	{
		tf::Taskflow TempTaskflow;
		TempTaskflow.for_each_index(std::forward<Iterator>(Begin), std::forward<Iterator>(End), Step, std::forward<Callable>(Function));
		if (WaitDone)
		{
			m_Executor->run(TempTaskflow).wait();
		}
		else
		{
			m_Executor->run(TempTaskflow);
		}
	}

	template<class Iterator, class CompareOp, bool8_t WaitDone = true>
	void ParallelSort(Iterator&& Begin, Iterator&& End, CompareOp&& Function)
	{
		tf::Taskflow TempTaskflow;
		TempTaskflow.sort(std::forward<Iterator>(Begin), std::forward<Iterator>(End), std::forward<CompareOp>(Function));
		if (WaitDone)
		{
			m_Executor->run(TempTaskflow).wait();
		}
		else
		{
			m_Executor->run(TempTaskflow);
		}
	}

	template<class Callable, bool8_t WaitDone = false>
	void Async(Callable&& Function)
	{
		if (WaitDone)
		{
			m_Executor->async(std::forward<Callable>(Function)).wait();
		}
		else
		{
			m_Executor->silent_async(std::forward<Callable>(Function));
		}
	}

	void RunTask(Task& InTask)
	{
		m_Executor->silent_async(std::string(InTask.GetName()), [&InTask]() {
			InTask.DoTask();
			});
	}

	template<bool8_t WaitDone = false>
	void RunTasks(std::vector<Task&>& InTasks)
	{
		tf::Taskflow TempTaskflow;
		for each (auto& Task in InTasks)
		{
			tf::Task TFTask = TempTaskflow.emplace([&Task]() {
				Task.DoTask();
				});
			TFTask
				.name(std::string(Task.GetName()))
				.priority(static_cast<tf::TaskPriority>(Task.GetPriority()));
		}
		if (WaitDone)
		{
			m_Executor->run(TempTaskflow).wait();
		}
		else
		{
			m_Executor->run(TempTaskflow);
		}
	}
private:
	std::unique_ptr<tf::Executor> m_Executor;
	bool8_t m_UseHyperThreading;
};