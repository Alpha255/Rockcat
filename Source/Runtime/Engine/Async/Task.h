#pragma once

#include "Runtime/Core/Definitions.h"

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

	template<class StringType>
	Task(StringType&& Name, ETaskType Type, EPriority Priority = EPriority::Normal)
		: m_Name(std::forward<StringType>(Name))
		, m_Type(Type)
		, m_Priority(Priority)
	{
	}

	const char8_t* GetName() const { return m_Name.c_str(); }
	ETaskType GetType() const { return m_Type; }
	EPriority GetPriority() const { return m_Priority; }

	virtual void DoTask() {}
private:
	std::string m_Name;
	ETaskType m_Type;
	EPriority m_Priority;
};

#define TF_ParallelFor(Begin, End, Callable) Engine::Get().GetTaskFlowModule().ParallelFor<true>(Begin, End, Callable)
#define TF_ParallelFor_NoWait(Begin, End, Callable) Engine::Get().GetTaskFlowModule().ParallelFor<false>(Begin, End, Callable)

#define TF_ParallelForIndex(Begin, End, Step, Callable) Engine::Get().GetTaskFlowModule().ParallelForIndex<Step, true>(Begin, End, Callable)
#define TF_ParallelForIndex_NoWait(Begin, End, Step, Callable) Engine::Get().GetTaskFlowModule().ParallelForIndex<Step, false>(Begin, End, Callable)

#define TF_ParallelSort(Begin, End, CompareOp) Engine::Get().GetTaskFlowModule().ParallelSort<true>(Begin, End, CompareOp)
#define TF_ParallelSort_NoWait(Begin, End, CompareOp) Engine::Get().GetTaskFlowModule().ParallelSort<false>(Begin, End, CompareOp)

#define TF_Async(Callable) Engine::Get().GetTaskFlowModule().Async<true>(Callable)
#define TF_Async_NoWait(Callable) Engine::Get().GetTaskFlowModule().Async<false>(Callable)

#define TF_DispatchTask(InTask) Engine::Get().GetTaskFlowModule().DispatchTask(InTask)

#define TF_DispatchTasks(InTasks) Engine::Get().GetTaskFlowModule().DispatchTasks<true>(InTasks)
#define TF_DispatchTasks_NoWait(InTasks) Engine::Get().GetTaskFlowModule().DispatchTasks<false>(InTasks)