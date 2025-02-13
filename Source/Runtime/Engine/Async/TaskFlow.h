#pragma once

#include "Core/PlatformMisc.h"

class TaskFlow : public tf::Taskflow
{
public:
	using tf::Taskflow::Taskflow;

	template<class TTask, class... Arg>
	TTask& Emplace(Arg&&... Args)
	{
		TTask* NewTask = Cast<TTask>(m_Tasks.emplace_back(std::make_shared<TTask>(std::forward<Arg>(Args)...)).get());

		NewTask->SetBaseTask(tf::Taskflow::emplace([NewTask]() {
			thread_local Task::EPriority LastPriority = Task::EPriority::Normal;
			bool NeedChangeThreadPriority = LastPriority != NewTask->GetPriority();
			if (NeedChangeThreadPriority)
			{
				PlatformMisc::SetThreadPriority(std::this_thread::get_id(), NewTask->GetPriority());
			}

			NewTask->Execute();

			if (NeedChangeThreadPriority)
			{
				PlatformMisc::SetThreadPriority(std::this_thread::get_id(), LastPriority);
				LastPriority = NewTask->GetPriority();
			}
		}));
		return *NewTask;
	}

	inline bool IsCompleted() const { return empty() ? true : (m_Event ? m_Event->IsCompleted() : true); }
	inline bool IsDispatched() const { return m_Event && m_Event->IsDispatched(); }
	
	inline TaskEventPtr GetEvent() const { return m_Event; }
	
	inline void Wait() 
	{
		if (IsDispatched())
		{
			m_Event->Wait();
		}
	}

	inline void WaitForSeconds(size_t Seconds)
	{
		if (IsDispatched())
		{
			m_Event->WaitForSeconds(Seconds);
		}
	}

	inline void WaitForMilliseconds(size_t Milliseconds)
	{
		if (IsDispatched())
		{
			m_Event->WaitForMilliseconds(Milliseconds);
		}
	}
protected:
	friend class TaskFlowService;

	inline void Execute(tf::Executor& Executor, size_t Repeat = 1u)
	{
		if (!empty())
		{
			auto Future = std::move(Executor.run_n(*this, Repeat));
			m_Event = std::make_shared<TaskEvent>(std::move(Future));
		}
	}

	template<class Pred, class Callback>
	inline void ExecuteUntil(tf::Executor& Executor, Pred&& Condition, Callback&& CompletedCallback, size_t Repeat = 1u)
	{
		if (!empty())
		{
			auto Future = Executor.run_until(*this,
				[Repeat, Condition]() mutable {
					return (Repeat-- == 0) && Condition();
				},
				std::forward<Callback>(CompletedCallback));
			m_Event = std::make_shared<TaskEvent>(std::move(Future));
		}
	}
private:
	std::vector<std::shared_ptr<tf::Task>> m_Tasks;
	TaskEventPtr m_Event;
	Task::EPriority m_LastTaskPriority = Task::EPriority::Normal;
};
