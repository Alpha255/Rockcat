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

	inline bool IsCompleted() const { return empty() ? true : m_Event.IsCompleted(); }
	inline bool IsDispatched() const { return m_Event.IsDispatched(); }
	
	inline const TaskEvent& GetEvent() const { return m_Event; }
	
	inline void Wait() 
	{
		if (IsDispatched())
		{
			m_Event.wait();
		}
	}

	inline void WaitFor(size_t Milliseconds)
	{
		if (IsDispatched())
		{
			m_Event.wait_for(std::chrono::milliseconds(Milliseconds));
		}
	}
protected:
	friend class TaskFlowService;

	inline void Execute(tf::Executor& Executor, size_t Repeat = 1u)
	{
		if (!empty())
		{
			m_Event = std::move(Executor.run_n(*this, Repeat));
		}
	}

	template<class Pred, class Callback>
	inline void ExecuteUntil(tf::Executor& Executor, Pred&& Condition, Callback&& CompletedCallback, size_t Repeat = 1u)
	{
		if (!empty())
		{
			m_Event = std::move(Executor.run_until(*this,
				[Repeat, Condition]() mutable {
					return (Repeat-- == 0) && Condition();
				},
				std::forward<Callback>(CompletedCallback)));
		}
	}
private:
	std::vector<std::shared_ptr<tf::Task>> m_Tasks;
	TaskEvent m_Event;
	Task::EPriority m_LastTaskPriority = Task::EPriority::Normal;
};
