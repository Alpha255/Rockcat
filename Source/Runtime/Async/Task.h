#pragma once

#include "Core/Name.h"

#pragma warning(push)
#pragma warning(disable:4324)
#include <taskflow/utility/traits.hpp>
#include <taskflow/taskflow.hpp>
#include <taskflow/core/task.hpp>
#include <taskflow/algorithm/for_each.hpp>
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
		Num
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

	TFTask(Name&& TaskName, EThread Thread = EThread::WorkerThread, EPriority Priority = EPriority::Normal)
		: m_Thread(Thread)
		, m_Priority(Priority)
		, m_Name(std::move(TaskName))
	{
	}

	template<class LAMBDA>
	TFTask(Name&& TaskName, LAMBDA&& Lambda, EThread Thread = EThread::WorkerThread, EPriority Priority = EPriority::Normal)
		: m_Thread(Thread)
		, m_Priority(Priority)
		, m_Name(std::move(TaskName))
		, m_TaskFunc(std::move([Func = std::forward<LAMBDA>(Lambda)]() { Func(); }))
	{
	}

	TFTask(TFTask&& Other) noexcept = delete;

	virtual ~TFTask();

	inline bool IsCompleted() const
	{
		return !m_Dispatched.load(std::memory_order_acquire) || m_Completed.load(std::memory_order_acquire);
	}

	inline bool IsDispatched() const { return m_Dispatched.load(std::memory_order_acquire); }

	inline bool IsCanceled() const { return m_Canceled.load(std::memory_order_acquire); }

	inline const Name& GetName() const { return m_Name; }

	void AddPrerequisite(TFTask& Prerequisite);

	bool Trigger();

	bool Restart();

	bool Wait();

	bool WaitForSeconds(size_t Seconds);

	bool WaitForMilliseconds(size_t Milliseconds);

	static void Initialize();
	static void Finalize();

	static bool IsGameThread();
	static bool IsRenderThread();
	static bool IsWorkerThread();

	static uint32_t GetNumWorkerThreads();

	template<class LAMBDA>
	static std::shared_ptr<TFTask> Launch(Name&& TaskName, LAMBDA&& Lambda, EThread Thread = EThread::WorkerThread, EPriority Priority = EPriority::Normal)
	{
		auto Task = std::make_shared<TFTask>(std::forward<Name>(TaskName), std::forward<LAMBDA>(Lambda), Thread, Priority);
		Task->Trigger();
		return Task;
	}

	template<class LAMBDA>
	static std::shared_ptr<TFTask> Launch(Name&& TaskName, LAMBDA&& Lambda, std::vector<TFTask*>&& PrerequisiteTasks, EThread Thread = EThread::WorkerThread, EPriority Priority = EPriority::Normal)
	{
		auto Task = std::make_shared<TFTask>(std::forward<Name>(TaskName), std::forward<LAMBDA>(Lambda), Thread, Priority);
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
		TFTaskFlow.for_each(Begin, End, std::forward<LAMBDA>(Lambda));
		return DispatchTaskFlow(std::move(TFTaskFlow), Thread, Priority);
	}

	template<class Iterator, class LAMBDA>
	static TFTaskEventPtr ParallelSort(Iterator&& Begin, Iterator&& End, LAMBDA&& Lambda, EThread Thread = EThread::WorkerThread, EPriority Priority = EPriority::Normal)
	{
		assert(Thread < EThread::Num);

		tf::Taskflow TFTaskFlow;
		TFTaskFlow.sort(Begin, End, std::forward<LAMBDA>(Lambda));
		return DispatchTaskFlow(std::move(TFTaskFlow), Thread, Priority);
	}
protected:
	static void InitializeThreadTags();

	static TFTaskEventPtr DispatchTaskFlow(tf::Taskflow&&, EThread Thread, EPriority Priority);

	void TriggerSubsequents();

	bool TryCancel();

	inline tf::AsyncTask GetAsyncTask() const
	{
		std::lock_guard<std::mutex> Locker(m_Lock);
		return m_AsyncTask;
	}

	inline void SetCanceled(bool Canceled) { m_Canceled.store(Canceled, std::memory_order_release); }

	virtual void Execute();

	inline friend bool IsSameExecutor(const TFTask& Task1, const TFTask& Task2) { return Task1.m_Thread == Task2.m_Thread && Task1.m_Priority == Task2.m_Priority; }

	inline bool HasAnyRef() const { return m_NumRef.load(std::memory_order_acquire) > 0u; }
	inline void AddRef() { m_NumRef.fetch_add(1u, std::memory_order_relaxed); }
	inline bool ReleaseRef()
	{
		uint32_t Current = m_NumRef.load(std::memory_order_acquire);

		while (Current > 0u)
		{
			if (m_NumRef.compare_exchange_weak(Current, Current - 1u, std::memory_order_acq_rel, std::memory_order_acquire))
			{
				return Current == 1u;
			}
		}

		assert(false);
		return false;
	}
private:
	EThread m_Thread = EThread::WorkerThread;
	EPriority m_Priority = EPriority::Normal;

	Name m_Name;

	std::unordered_set<TFTask*> m_Prerequisites;
	std::unordered_set<TFTask*> m_Subsequents;

	mutable std::mutex m_Lock;
	std::atomic<bool> m_Canceled{ false };
	std::atomic<bool> m_Dispatched{ false };
	std::atomic<bool> m_Completed{ false };

	std::atomic<uint32_t> m_NumRef{ 0u };

	std::function<void()> m_TaskFunc;

	tf::AsyncTask m_AsyncTask;
	std::shared_ptr<std::future<void>> m_Future;
};