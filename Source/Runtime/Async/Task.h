#pragma once

#include "Core/Name.h"
#include "Core/Singleton.h"

#pragma warning(push)
#pragma warning(disable:4456 4244 4127 4267 4324)
#include <taskflow/taskflow/utility/traits.hpp>
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
			m_Future.get();
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
		, m_TaskFunc(std::move([Func = std::forward<LAMBDA>(Lambda)]() { Func(); }))
	{
	}

	TFTask(TFTask&& Other) noexcept
		: m_Thread(Other.m_Thread)
		, m_Priority(Other.m_Priority)
		, m_Name(std::move(Other.m_Name))
		, m_TaskFunc(std::move(Other.m_TaskFunc))
	{
	}

	~TFTask();

	inline bool IsCompleted() const { return m_TFAsyncTask ? m_TFAsyncTask->first.is_done() : false; }
	inline bool IsDispatched() const { return m_TFAsyncTask ? true : false; }
	inline bool IsCanceled() const { return m_Canceled.load(std::memory_order_acquire); }

	inline const FName& GetName() const { return m_Name; }

	void AddPrerequisite(TFTask& Prerequisite);

	bool Trigger();

	inline bool Wait() 
	{
		if (m_TFAsyncTask)
		{
			m_TFAsyncTask->second.get();
			return true;
		}

		return false;
	}

	inline bool WaitForSeconds(size_t Seconds) 
	{
		if (m_TFAsyncTask)
		{
			return m_TFAsyncTask->second.wait_for(std::chrono::seconds(Seconds)) == std::future_status::ready;
		}

		return false;
	}

	inline bool WaitForMilliseconds(size_t Milliseconds)
	{
		if (m_TFAsyncTask)
		{
			return m_TFAsyncTask->second.wait_for(std::chrono::milliseconds(Milliseconds)) == std::future_status::ready;
		}

		return false;
	}

	static void Initialize();
	static void Finalize();

	static bool IsMainThread();
	static bool IsGameThread();
	static bool IsRenderThread();
	static bool IsWorkerThread();

	static uint32_t GetNumWorkerThreads();

	template<class LAMBDA>
	static std::shared_ptr<TFTask> Launch(FName&& Name, LAMBDA&& Lambda, EThread Thread = EThread::WorkerThread, EPriority Priority = EPriority::Normal)
	{
		auto Task = std::make_shared<TFTask>(std::forward<FName>(Name), std::forward<LAMBDA>(Lambda), Thread, Priority);
		Task->Trigger();
		return Task;
	}

	template<class LAMBDA>
	static std::shared_ptr<TFTask> Launch(FName&& Name, LAMBDA&& Lambda, std::vector<TFTask*>&& PrerequisiteTasks, EThread Thread = EThread::WorkerThread, EPriority Priority = EPriority::Normal)
	{
		auto Task = std::make_shared<TFTask>(std::forward<FName>(Name), std::forward<LAMBDA>(Lambda), Thread, Priority);
		for (auto PrerequisiteTask : PrerequisiteTasks)
		{
			Task->AddPrerequisite(*PrerequisiteTask);
		}

		Task->Trigger();
		return Task;
	}

	template<class Iterator, class LAMBDA>
	static TFTaskEventPtr ParallelFor(Iterator&& Begin, Iterator&& End, LAMBDA&& Lambda, EThread Thread = EThread::WorkerThread, EPriority Priority = EPriority::Normal)
	{
		assert(Thread < EThread::Num);

		tf::Taskflow TFTaskFlow;
		TFTaskFlow.for_each(std::forward<Iterator>(Begin), std::forward<Iterator>(End), std::forward<LAMBDA>(Lambda));
		return DispatchTaskFlow(std::move(TFTaskFlow), Thread, Priority);
	}

	template<class Iterator, class LAMBDA>
	static TFTaskEventPtr ParallelSort(Iterator&& Begin, Iterator&& End, LAMBDA&& Lambda, EThread Thread = EThread::WorkerThread, EPriority Priority = EPriority::Normal)
	{
		assert(Thread < EThread::Num);

		tf::Taskflow TFTaskFlow;
		TFTaskFlow.sort(std::forward<Iterator>(Begin), std::forward<Iterator>(End), std::forward<LAMBDA>(Lambda));
		return DispatchTaskFlow(std::move(TFTaskFlow), Thread, Priority);
	}
protected:
	static void InitializeThreadTags();

	static TFTaskEventPtr DispatchTaskFlow(tf::Taskflow&&, EThread Thread, EPriority Priority);

	void AddSubsequents(TFTask& Subsequent);

	void TriggerSubsequents();

	bool TryCancel();

	inline tf::AsyncTask* GetAsyncTask() { return &m_TFAsyncTask->first; }

	inline void SetCanceled(bool Canceled) { m_Canceled.store(Canceled, std::memory_order_release); }

	virtual void Execute();

	inline friend bool IsSameExecutor(const TFTask& Task1, const TFTask& Task2) { return Task1.m_Thread == Task2.m_Thread && Task1.m_Priority == Task2.m_Priority; }

	inline bool HasAnyRef() const { return m_NumRef.load(std::memory_order_acquire) > 0u; }
	inline void AddRef() { m_NumRef.fetch_add(1u, std::memory_order_relaxed); }
	inline bool ReleaseRef()
	{
		assert(m_NumRef.load(std::memory_order_acquire) > 0u);
		return m_NumRef.fetch_sub(1u, std::memory_order_acq_rel) == 1u;
	}
private:
	using TFAsyncTask = std::pair<tf::AsyncTask, std::future<void>>;

	EThread m_Thread = EThread::WorkerThread;
	EPriority m_Priority = EPriority::Normal;

	FName m_Name;

	std::unordered_set<TFTask*> m_Prerequisites;
	std::unordered_set<TFTask*> m_Subsequents;

	std::mutex m_Lock;
	std::atomic<bool> m_Canceled;

	std::atomic<uint32_t> m_NumRef{ 0u };

	std::function<void()> m_TaskFunc;

	std::shared_ptr<TFAsyncTask> m_TFAsyncTask;
};