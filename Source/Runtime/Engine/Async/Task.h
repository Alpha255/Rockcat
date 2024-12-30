#pragma once

#include "Core/Definitions.h"

#pragma warning(push)
#pragma warning(disable:4456 4244 4127 4267 4324)
#include <Submodules/taskflow/taskflow/taskflow.hpp>
#include <Submodules/taskflow/taskflow/core/task.hpp>
#include <Submodules/taskflow/taskflow/algorithm/for_each.hpp>
#include <Submodules/taskflow/taskflow/algorithm/sort.hpp>
#pragma warning(pop)

class Task
{
public:
	enum class EPriority
	{
		Critical,
		High,
		Normal,
		Low,
	};

	Task() = default;

	template<class T>
	Task(T&& Name, EPriority Priority = EPriority::Normal)
		: m_Name(std::forward<T>(Name))
		, m_Priority(Priority)
	{
	}

	const char* GetName() const { return m_Name.c_str(); }
	
	template<class T>
	Task& SetName(T&& Name) { m_Name = std::string(std::move(Name)); return *this; }

	EPriority GetPriority() const { return m_Priority; }
	Task& SetPriority(EPriority Priority) { m_Priority = Priority; return *this; }

	virtual void Execute() = 0;
private:
	std::string m_Name;
	EPriority m_Priority = EPriority::Normal;
};

class TaskFlow : public tf::Taskflow
{
public:
	using tf::Taskflow::Taskflow;

	void EmplaceTask(Task& InTask)
	{
		auto TFTask = emplace([&InTask]() {
				InTask.Execute();
			});
		TFTask.name(std::string(InTask.GetName()));
	}

	void EmplaceTask(Task&& InTask)
	{
		auto TFTask = emplace([&InTask]() {
			InTask.Execute();
			});
		TFTask.name(std::string(InTask.GetName()));
	}

};

#define TF_ParallelFor_WaitDone(Begin, End, Callable, Thread) TaskFlowService::Get().ParallelFor<true>(Begin, End, Callable, Thread)
#define TF_ParallelFor(Begin, End, Callable, Thread) TaskFlowService::Get().ParallelFor<false>(Begin, End, Callable, Thread)

#define TF_ParallelForRange_WaitDone(Begin, End, Callable, Thread) TaskFlowService::Get().ParallelForRange<1u, true>(Begin, End, Callable, Thread)
#define TF_ParallelForRange(Begin, End, Callable, Thread) TaskFlowService::Get().ParallelForRange<1u, false>(Begin, End, Callable, Thread)

#define TF_ParallelSort_WaitDone(Begin, End, CompareOp, Thread) TaskFlowService::Get().ParallelSort<true>(Begin, End, CompareOp, Thread)
#define TF_ParallelSort(Begin, End, CompareOp, Thread) TaskFlowService::Get().ParallelSort<false>(Begin, End, CompareOp, Thread)

#define TF_Async_WaitDone(Callable, Thread) TaskFlowService::Get().Async<true>(Callable, Thread)
#define TF_Async(Callable, Thread) TaskFlowService::Get().Async<false>(Callable, Thread)

#define TF_DispatchTask_WaitDone(InTask, Thread) TaskFlowService::Get().DispatchTask<true>(InTask, Thread)
#define TF_DispatchTask(InTask, Thread) TaskFlowService::Get().DispatchTask<false>(InTask, Thread)

#define TF_DispatchTasks_WaitDone(InTasks, Thread) TaskFlowService::Get().DispatchTasks<true>(InTasks, Thread)
#define TF_DispatchTasks(InTasks, Thread) TaskFlowService::Get().DispatchTasks<false>(InTasks, Thread)