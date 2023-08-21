#pragma once

#include "Runtime/Core/Module.h"
#include "Runtime/Engine/Async/Task.h"

#pragma warning(push)
#pragma warning(disable:4244)
#pragma warning(disable:4456)
#include <Submodules/taskflow/taskflow/taskflow.hpp>
#include <Submodules/taskflow/taskflow/algorithm/for_each.hpp>
#include <Submodules/taskflow/taskflow/algorithm/sort.hpp>
#pragma warning(pop)

class TaskFlowService : public IService<TaskFlowService>
{
public:
	TaskFlowService();

	template<bool8_t WaitDone = true, class Iterator, class Callable>
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

	template<size_t Step, bool8_t WaitDone = true, class Callable>
	void ParallelForIndex(size_t Begin, size_t End, Callable&& Function)
	{
		assert(Step < (End - Begin));

		tf::Taskflow TempTaskflow;
		TempTaskflow.for_each_index(Begin, End, Step, std::forward<Callable>(Function));
		if (WaitDone)
		{
			m_Executor->run(TempTaskflow).wait();
		}
		else
		{
			m_Executor->run(TempTaskflow);
		}
	}

	template<bool8_t WaitDone = true, class Iterator, class CompareOp>
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

	template<bool8_t WaitDone = false, class Callable>
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

	void DispatchTask(Task& InTask)
	{
		m_Executor->silent_async(std::string(InTask.GetName()), [&InTask]() {
			InTask.DoTask();
			});
	}

	template<bool8_t WaitDone = false>
	void DispatchTasks(std::vector<Task&>& InTasks)
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