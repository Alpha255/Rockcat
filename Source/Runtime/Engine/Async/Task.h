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
	enum class ETaskType
	{
		General,
		Graphics,
		ShaderCompile
	};

	enum EPriority
	{
		High,
		Normal,
		Low,
	};

	template<class T>
	Task(T&& Name, ETaskType Type, EPriority Priority = EPriority::Normal)
		: m_Name(std::forward<T>(Name))
		, m_Type(Type)
		, m_Priority(Priority)
	{
	}

	const char* GetName() const { return m_Name.c_str(); }
	ETaskType GetType() const { return m_Type; }
	EPriority GetPriority() const { return m_Priority; }

	virtual void Execute() {}
private:
	std::string m_Name;
	ETaskType m_Type;
	EPriority m_Priority;
};

#define TF_ParallelFor(Begin, End, Callable, Thread) TaskFlowService::Get().ParallelFor<true>(Begin, End, Callable, Thread)
#define TF_ParallelFor_NoWait(Begin, End, Callable, Thread) TaskFlowService::Get().ParallelFor<false>(Begin, End, Callable, Thread)

#define TF_ParallelForIndex(Begin, End, Step, Callable, Thread) TaskFlowService::Get().ParallelForIndex<Step, true>(Begin, End, Callabl, Thread)
#define TF_ParallelForIndex_NoWait(Begin, End, Step, Callable, Thread) TaskFlowService::Get().ParallelForIndex<Step, false>(Begin, End, Callable, Thread)

#define TF_ParallelSort(Begin, End, CompareOp, Thread) TaskFlowService::Get().ParallelSort<true>(Begin, End, CompareOp, Thread)
#define TF_ParallelSort_NoWait(Begin, End, CompareOp, Thread) TaskFlowService::Get().ParallelSort<false>(Begin, End, CompareOp, Thread)

#define TF_Async(Callable, Thread) TaskFlowService::Get().Async<true>(Callable, Thread)
#define TF_Async_NoWait(Callable, Thread) TaskFlowService::Get().Async<false>(Callable, Thread)

#define TF_DispatchTask(InTask, Thread) TaskFlowService::Get().DispatchTask(InTask, Thread)

#define TF_DispatchTasks(InTasks, Thread) TaskFlowService::Get().DispatchTasks<true>(InTasks, Thread)
#define TF_DispatchTasks_NoWait(InTasks, Thread) TaskFlowService::Get().DispatchTasks<false>(InTasks, Thread)