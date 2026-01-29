#pragma once

#include "Core/Definitions.h"
#include "Core/Name.h"

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

class Future : public tf::Future<void>
{
public:
	using tf::Future<void>::Future;

	explicit Future(tf::Future<void>& Future) noexcept
	{
		*this = std::move(Future);
	}

	explicit Future(std::future<void>& Future) noexcept
	{
		*this = std::move(Future);
	}
};

class TaskEvent
{
public:
	TaskEvent() = default;

	TaskEvent(const TaskEvent&) = delete;
	
	TaskEvent(TaskEvent&& Other) noexcept
		: m_Future(std::move(Other.m_Future))
	{
	}

	TaskEvent& operator=(const TaskEvent&) = delete;

	TaskEvent& operator=(TaskEvent&& Other) noexcept
	{
		if (this != &Other)
		{
			m_Future = std::move(Other.m_Future);
		}
		return *this;
	}

	explicit TaskEvent(tf::Future<void>& Future) noexcept
		: m_Future(Future)
	{
	}

	explicit TaskEvent(std::future<void>& Future) noexcept
		: m_Future(Future)
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

	inline bool Cancel()
	{
		return m_Future.valid() ? m_Future.cancel() : false;
	}
private:
	Future m_Future;
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
	
	TaskFlowTask(FName&& Name, EPriority Priority = EPriority::Normal)
		: m_Priority(Priority)
		, m_Name(std::move(Name))
	{
		// The node is null if it is not yet emplaced in a taskflow
		//tf::Task::name(m_Name);
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
		//tf::Task::name(m_Name);
	}

	~TaskFlowTask() = default;

	virtual void Dispatch(EThread Thread = EThread::WorkerThread) = 0;

	inline const FName& GetName() const { return m_Name; }
	inline void SetName(FName&& Name)
	{
		m_Name = std::move(Name);
		tf::Task::name(m_Name.Get().data());
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

	inline bool Cancel()
	{
		return m_Event ? m_Event->Cancel() : false;
	}
protected:
	friend class TaskFlow;

	static void InitializeThreadTags();

	bool m_Executing = false;
	EPriority m_Priority = EPriority::Normal;

	FName m_Name;

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

	inline bool Cancel()
	{
		return m_Event ? m_Event->Cancel() : false;
	}
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

class TTaskEvent
{
public:
	TTaskEvent(std::future<void>&& Future)
		: m_Future(std::move(Future))
	{
	}

	inline void Wait() { m_Future.wait(); }
	inline void WaitFor(size_t Milliseconds) { m_Future.wait_for(std::chrono::milliseconds(Milliseconds)); }
private:
	std::future<void> m_Future;
};

class TTask : public NoneCopyable
{
public:
	enum class EPriority : uint8_t
	{
		Low,
		Normal,
		High,
		Critical
	};

	TTask(FName&& Name, EThread Thread = EThread::WorkerThread, EPriority Priority = EPriority::Normal)
		: m_Thread(Thread)
		, m_Priority(Priority)
		, m_Name(std::move(Name))
	{
	}

	template<class TaskBodyType>
	TTask(FName&& Name, TaskBodyType&& TaskBody, EThread Thread = EThread::WorkerThread, EPriority Priority = EPriority::Normal)
		: m_Thread(Thread)
		, m_Priority(Priority)
		, m_Name(std::move(Name))
	{
	}

	TTask(const TTask&) = delete;

	TTask& operator=(const TTask&) = delete;

	TTask(TTask&& Other) noexcept
		: m_Thread(Other.m_Thread)
		, m_Priority(Other.m_Priority)
		, m_Name(std::move(Other.m_Name))
	{
	}

	~TTask() = default;

	inline bool IsCompleted() const { return m_LowLevelTask ? m_LowLevelTask->IsDone() : false; }
	inline bool IsDispatched() const { return m_LowLevelTask ? m_LowLevelTask->IsValid() : false; }

	inline const FName& GetName() const { return m_Name; }

	void Launch();
protected:
	bool TryLaunch();
	bool TryCancel();

	bool AddPrerequisite(TTask& Prerequisite)
	{
		if (!Prerequisite.AddSubsequents(*this))
		{
			return false;
		}

		std::lock_guard<std::mutex> Locker(m_Lock);
		m_Prerequisites.insert(&Prerequisite);

		return true;
	}

	bool AddSubsequents(TTask& Subsequent)
	{
		std::lock_guard<std::mutex> Locker(m_Lock);
		m_Subsequents.insert(&Subsequent);
	}

	virtual void Execute() = 0;
private:
	EThread m_Thread = EThread::WorkerThread;
	EPriority m_Priority = EPriority::Normal;

	FName m_Name;

	std::unordered_set<TTask*> m_Prerequisites;
	std::unordered_set<TTask*> m_Subsequents;

	std::mutex m_Lock;

	std::function<void()> m_TaskBody;

	struct TFTask : public std::pair<tf::AsyncTask, std::future<void>>
	{
		using std::pair<tf::AsyncTask, std::future<void>>::pair;

		inline bool IsValid() const { return second.valid(); }
		inline bool IsDone() const { return first.is_done(); }

		inline tf::AsyncTask& GetTask() { return first; }
		inline std::future<void>& GetFuture() { return second; }
	};

	std::shared_ptr<TFTask> m_LowLevelTask;
};