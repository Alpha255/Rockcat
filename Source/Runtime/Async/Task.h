#pragma once

#include "Core/Definitions.h"

#pragma warning(push)
#pragma warning(disable:4456 4244 4127 4267 4324)
#include <Submodules/taskflow/taskflow/taskflow.hpp>
#include <Submodules/taskflow/taskflow/core/task.hpp>
#include <Submodules/taskflow/taskflow/algorithm/for_each.hpp>
#include <Submodules/taskflow/taskflow/algorithm/sort.hpp>
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

class ITask
{
public:
	enum class EPriority : uint8_t
	{
		Low,
		Normal,
		High,
		Critical
	};

	ITask() = default;
	
	ITask(const std::string& Name, EPriority Priority = EPriority::Normal)
		: m_Priority(Priority)
		, m_Name(Name)
	{
	}

	ITask(const ITask& Other)
		: m_Executing(Other.m_Executing)
		, m_Priority(Other.m_Priority)
		, m_Name(Other.m_Name)
		, m_Event(Other.m_Event)
	{
	}

	ITask& operator=(const ITask& Other)
	{
		m_Executing = Other.m_Executing;
		m_Priority = Other.m_Priority;
		m_Name = Other.m_Name;
		m_Event = Other.m_Event;
		return *this;
	}

	ITask(ITask&& Other) noexcept
		: m_Executing(Other.m_Executing)
		, m_Priority(Other.m_Priority)
		, m_Name(std::move(Other.m_Name))
		, m_Event(std::move(Other.m_Event))
	{
	}

	~ITask() = default;

	virtual void Dispatch(EThread Thread = EThread::WorkerThread) = 0;

	inline const char* GetName() const { return m_Name.c_str(); }
	inline EPriority GetPriority() const { return m_Priority; }
	inline bool IsDispatched() const { return m_Event && m_Event->IsDispatched(); }
	inline bool IsCompleted() const { return IsDispatched() ? m_Event->IsCompleted() : !m_Executing; }
	inline TaskEventPtr GetEvent() const { return m_Event; }
	
	virtual void SetName(const char* Name) { m_Name = Name; }

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

	static void InitializeThreadTags();
	static bool IsInMainThread();
	static bool IsInGameThread();
	static bool IsInRenderThread();
	static bool IsInWorkerThread();
protected:
	friend class TaskFlowService;

	bool m_Executing = false;
	EPriority m_Priority = EPriority::Normal;

	std::string m_Name;

	TaskEventPtr m_Event;
};

class Task : public tf::Task, public ITask
{
public:
	using tf::Task::Task;
	using ITask::ITask;

	Task() = default;

	Task(const Task& Other)
		: tf::Task(Other)
		, ITask(Other)
	{
		SetBaseName();
	}

	Task(Task&& Other) noexcept
		: tf::Task(std::move(static_cast<tf::Task&&>(Other)))
		, ITask(std::move(Other))
	{
		SetBaseName();
	}

	Task& operator=(const Task& Other)
	{
		ITask::operator=(Other);
		SetBaseName();
		return *this;
	}

	void SetName(const char* Name) override final
	{
		ITask::SetName(Name);
		SetBaseName();
	}

	void Execute();

	void Dispatch(EThread Thread = EThread::WorkerThread) override final;
protected:
	friend class TaskFlow;
	friend class TaskFlowService;

	virtual void ExecuteImpl() = 0;

	inline void SetBaseTask(tf::Task&& Task)
	{
		Cast<tf::Task>(*this) = Task;
		SetBaseName();
	}
private:
	void SetBaseName()
	{
#if _DEBUG
		if (!empty())
		{
			tf::Task::name(m_Name);
		}
#endif
	}
};

class TaskFlow : public tf::Taskflow, public ITask
{
public:
	using tf::Taskflow::Taskflow;
	using ITask::ITask;

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
	friend class TaskFlowService;
private:
	std::vector<std::shared_ptr<tf::Task>> m_Tasks;
};