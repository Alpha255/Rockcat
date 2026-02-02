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

class TFTaskEvent
{
public:
	TFTaskEvent() = delete;
	TFTaskEvent(const TFTaskEvent&) = delete;
	TFTaskEvent(TFTaskEvent&& Other) noexcept = default;
	TFTaskEvent& operator=(const TFTaskEvent&) = delete;
	TFTaskEvent& operator=(TFTaskEvent&& Other) noexcept = default;

	TFTaskEvent(std::future<void>&& Future) noexcept
		: m_Future(std::move(Future))
	{
	}
	
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
	std::weak_ptr<class TFTask> m_Task;
};
using TFTaskEventPtr = std::shared_ptr<TFTaskEvent>;

class TFTask : public NoneCopyable
{
public:
	enum class EThread
	{
		GameThread,
		RenderThread,
		WorkerThread,
		Num,
		MainThread
	};

	enum class EPriority : uint8_t
	{
		Low,
		Normal,
		High,
		Critical
	};

	enum class EState : uint8_t
	{
		None,
		Dispatched,
		Canceled,
		Completed
	};

	TFTask(FName&& Name, EThread Thread = EThread::WorkerThread, EPriority Priority = EPriority::Normal)
		: m_Thread(Thread)
		, m_Priority(Priority)
		, m_Name(std::move(Name))
	{
	}

	template<class LAMBDA>
	TFTask(FName&& Name, LAMBDA&& Lambda, EThread Thread = EThread::WorkerThread, EPriority Priority = EPriority::Normal)
		: m_Thread(Thread)
		, m_Priority(Priority)
		, m_Name(std::move(Name))
		, m_TaskFunc([Func=std::forward<LAMBDA>(Lambda)]() {
			Func() })
	{
	}

	TFTask(TFTask&& Other) noexcept
		: m_Thread(Other.m_Thread)
		, m_Priority(Other.m_Priority)
		, m_Name(std::move(Other.m_Name))
		, m_TaskFunc(std::move(Other.m_TaskFunc))
	{
	}

	~TFTask() = default;

	inline bool IsCompleted() const { return m_AsyncTask ? m_AsyncTask->is_done() : false; }
	inline bool IsDispatched() const { return m_Event ? true : false; }
	inline bool IsCanceled(std::memory_order MemoryOrder = std::memory_order_relaxed) const { return m_Canceled.load(MemoryOrder); }

	inline const FName& GetName() const { return m_Name; }

	bool AddPrerequisite(TFTask& Prerequisite);

	void Launch();

	static bool IsMainThread();
	static bool IsGameThread();
	static bool IsRenderThread();
	static bool IsWorkerThread();

	template<class LAMBDA>
	static std::shared_ptr<TFTask> Launch(FName&& Name, LAMBDA&& Lambda, EThread Thread = EThread::WorkerThread, EPriority Priority = EPriority::Normal)
	{
		auto Task = std::make_shared<TFTask>(std::forward<FName>(Name), std::forward<LAMBDA>(Lambda), Thread, Priority);
		Task->Launch();
		return Task;
	}

	template<class LAMBDA, class Prerequisites>
	static std::shared_ptr<TFTask> Launch(FName&& Name, LAMBDA&& Lambda, EThread Thread = EThread::WorkerThread, EPriority Priority = EPriority::Normal)
	{

	}
protected:
	static void InitializeThreadTags();

	bool AddSubsequents(TFTask& Subsequent);

	bool TryCancel();

	inline tf::AsyncTask* GetAsyncTask() { return m_AsyncTask.get(); }

	inline void SetCanceled(bool Canceled, std::memory_order MemoryOrder = std::memory_order_relaxed) { m_Canceled.store(Canceled, MemoryOrder); }

	virtual void Execute();
private:
	EThread m_Thread = EThread::WorkerThread;
	EPriority m_Priority = EPriority::Normal;

	FName m_Name;

	std::unordered_set<TFTask*> m_Prerequisites;
	std::unordered_set<TFTask*> m_Subsequents;

	std::mutex m_Lock;
	std::atomic<bool> m_Canceled;

	std::function<void()> m_TaskFunc;

	std::shared_ptr<tf::AsyncTask> m_AsyncTask;
	std::shared_ptr<TFTaskEvent> m_Event;
};