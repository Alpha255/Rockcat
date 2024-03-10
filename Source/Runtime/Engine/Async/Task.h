#pragma once

#include "Core/Definitions.h"

#pragma warning(disable:4456)
#include <Submodules/taskflow/taskflow/core/task.hpp>
#pragma warning(default:4456)

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
		High = tf::TaskPriority::HIGH,
		Normal = tf::TaskPriority::NORMAL,
		Low = tf::TaskPriority::LOW,
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

#define TF_ParallelFor(Begin, End, Callable) TaskFlowService::Get().ParallelFor<true>(Begin, End, Callable)
#define TF_ParallelFor_NoWait(Begin, End, Callable) TaskFlowService::Get().ParallelFor<false>(Begin, End, Callable)

#define TF_ParallelForIndex(Begin, End, Step, Callable) TaskFlowService::Get().ParallelForIndex<Step, true>(Begin, End, Callable)
#define TF_ParallelForIndex_NoWait(Begin, End, Step, Callable) TaskFlowService::Get().ParallelForIndex<Step, false>(Begin, End, Callable)

#define TF_ParallelSort(Begin, End, CompareOp) TaskFlowService::Get().ParallelSort<true>(Begin, End, CompareOp)
#define TF_ParallelSort_NoWait(Begin, End, CompareOp) TaskFlowService::Get().ParallelSort<false>(Begin, End, CompareOp)

#define TF_Async(Callable) TaskFlowService::Get().Async<true>(Callable)
#define TF_Async_NoWait(Callable) TaskFlowService::Get().Async<false>(Callable)

#define TF_DispatchTask(InTask) TaskFlowService::Get().DispatchTask(InTask)

#define TF_DispatchTasks(InTasks) TaskFlowService::Get().DispatchTasks<true>(InTasks)
#define TF_DispatchTasks_NoWait(InTasks) TaskFlowService::Get().DispatchTasks<false>(InTasks)