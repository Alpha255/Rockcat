#pragma once

#include "Core/Definitions.h"

#pragma warning(push)
#pragma warning(disable:4456 4244 4127 4267 4324)
#include <taskflow/taskflow.hpp>
#include <taskflow/core/task.hpp>
#include <taskflow/algorithm/for_each.hpp>
#include <taskflow/algorithm/sort.hpp>
#pragma warning(pop)

enum class EThread
{
	GameThread,
	RenderThread,
	WorkerThread,
	Num,
	MainThread
};

class TaskEvent
{
public:
	TaskEvent() = default;

	TaskEvent(std::future<void>&& Future) noexcept
		: m_Future(std::move((Future)))
	{
	}

	inline bool IsDispatched() const { return m_Future.valid(); }
	inline bool IsCompleted() const { return m_Future.valid() ? m_Future.wait_for(std::chrono::milliseconds(0u)) == std::future_status::ready : true; }
	
	inline void Wait() 
	{
		if (m_Future.valid())
		{
			m_Future.wait();
		}
	}

	inline void WaitForSeconds(size_t Seconds) 
	{
		if (m_Future.valid())
		{
			m_Future.wait_for(std::chrono::seconds(Seconds));
		}
	}

	inline void WaitForMilliseconds(size_t Milliseconds)
	{
		if (m_Future.valid())
		{
			m_Future.wait_for(std::chrono::milliseconds(Milliseconds));
		}
	}
private:
	std::future<void> m_Future;
};

using TaskEventPtr = std::shared_ptr<TaskEvent>;

class TaskFlowTask : public tf::Task
{
public:
	enum class EPriority : uint8_t
	{
		Low,
		Normal,
		High,
		Critical
	};

	TaskFlowTask() = default;
	
	TaskFlowTask(const std::string& Name, EPriority Priority = EPriority::Normal)
		: m_Priority(Priority)
		, m_Name(Name)
	{
		tf::Task::name(m_Name);
	}

	TaskFlowTask(const TaskFlowTask& Other)
		: m_Executing(Other.m_Executing)
		, m_Priority(Other.m_Priority)
		, m_Name(Other.m_Name)
		, m_Event(Other.m_Event)
	{
	}

	TaskFlowTask& operator=(const TaskFlowTask& Other)
	{
		m_Executing = Other.m_Executing;
		m_Priority = Other.m_Priority;
		m_Name = Other.m_Name;
		m_Event = Other.m_Event;
		return *this;
	}

	TaskFlowTask(TaskFlowTask&& Other) noexcept
		: m_Executing(Other.m_Executing)
		, m_Priority(Other.m_Priority)
		, m_Name(std::move(Other.m_Name))
		, m_Event(std::move(Other.m_Event))
	{
		tf::Task::name(m_Name);
	}

	~TaskFlowTask() = default;

	virtual void Dispatch(EThread Thread = EThread::WorkerThread) = 0;

	inline const char* GetName() const { return m_Name.c_str(); }
	inline void SetName(const char* Name)
	{
		m_Name = Name;
		tf::Task::name(m_Name);
	}

	inline EPriority GetPriority() const { return m_Priority; }
	inline bool IsDispatched() const { return m_Event && m_Event->IsDispatched(); }
	inline bool IsCompleted() const { return IsDispatched() ? m_Event->IsCompleted() : !m_Executing; }
	inline TaskEventPtr GetEvent() const { return m_Event; }

	inline void Wait()
	{
		if (m_Event)
		{
			m_Event->Wait();
		}
	}

	inline void WaitForSeconds(size_t Seconds)
	{
		if (m_Event)
		{
			m_Event->WaitForSeconds(Seconds);
		}
	}

	inline void WaitForMilliseconds(size_t Milliseconds)
	{
		if (m_Event)
		{
			m_Event->WaitForMilliseconds(Milliseconds);
		}
	}
protected:
	friend class TaskFlow;

	static void InitializeThreadTags();

	bool m_Executing = false;
	EPriority m_Priority = EPriority::Normal;

	std::string m_Name;

	TaskEventPtr m_Event;
};

class Task : public TaskFlowTask
{
public:
	using TaskFlowTask::TaskFlowTask;

	static bool IsMainThread();
	static bool IsGameThread();
	static bool IsRenderThread();
	static bool IsWorkerThread();

	void Execute();

	void Dispatch(EThread Thread = EThread::WorkerThread) override final;
protected:
	friend class TaskFlow;

	virtual void ExecuteImpl() = 0;

	inline void SetBaseTask(tf::Task&& Task)
	{
		Cast<tf::Task>(*this) = Task;
	}
};

class TaskSet : public tf::Taskflow, public TaskFlowTask
{
public:
	using tf::Taskflow::Taskflow;
	using TaskFlowTask::TaskFlowTask;

	template<class TTask, class... Arg>
	TTask& Emplace(Arg&&... Args)
	{
		TTask* NewTask = Cast<TTask>(m_Tasks.emplace_back(std::make_shared<TTask>(std::forward<Arg>(Args)...)).get());

		NewTask->SetBaseTask(tf::Taskflow::emplace([NewTask]() {
			NewTask->Execute();
			}));
		return *NewTask;
	}

	void Dispatch(EThread Thread = EThread::WorkerThread) override final;
protected:
	friend class TaskFlow;
private:
	std::vector<std::shared_ptr<tf::Task>> m_Tasks;
};