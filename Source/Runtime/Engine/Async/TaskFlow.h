#pragma once

#include "Engine/Async/Task.h"

class TaskFlow : public tf::Taskflow
{
public:
	using tf::Taskflow::Taskflow;

	template<class TTask, class... Arg>
	TTask& Emplace(Arg&&... Args)
	{
		TTask* NewTask = Cast<TTask>(m_Tasks.emplace_back(std::make_shared<TTask>(std::forward<Arg>(Args)...)).get());

		NewTask->SetBaseTask(tf::Taskflow::emplace([NewTask]() {
			NewTask->Execute();
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

	inline void Dispatch(tf::Executor& Executor)
	{
		m_Event = std::make_shared<TaskEvent>(std::move(std::move(Executor.run(*this))));
	}
private:
	std::vector<std::shared_ptr<tf::Task>> m_Tasks;
	TaskEventPtr m_Event;
};
