#pragma once

#include "Core/Name.h"
#include "Core/SpdLogging.h"
#include "Async/TaskEvent.h"

#pragma warning(push)
#pragma warning(disable:4324)
#include <taskflow/utility/traits.hpp>
#include <taskflow/taskflow.hpp>
#include <taskflow/core/task.hpp>
#include <taskflow/algorithm/for_each.hpp>
#pragma warning(pop)

class TFTask : public NoneCopyable
{
public:
	TFTask(Name&& TaskName, ETFTaskThread Thread = ETFTaskThread::WorkerThread, ETFTaskPriority Priority = ETFTaskPriority::Normal)
		: m_Thread(Thread)
		, m_Priority(Priority)
		, m_Name(std::move(TaskName))
	{
	}

	template<class LAMBDA>
	TFTask(Name&& TaskName, LAMBDA&& Lambda, ETFTaskThread Thread = ETFTaskThread::WorkerThread, ETFTaskPriority Priority = ETFTaskPriority::Normal)
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

	static bool IsGameThread();
	static bool IsRenderThread();
	static bool IsWorkerThread();

	template<class LAMBDA>
	static std::shared_ptr<TFTask> Launch(Name&& TaskName, LAMBDA&& Lambda, ETFTaskThread Thread = ETFTaskThread::WorkerThread, ETFTaskPriority Priority = ETFTaskPriority::Normal)
	{
		auto Task = std::make_shared<TFTask>(std::forward<Name>(TaskName), std::forward<LAMBDA>(Lambda), Thread, Priority);
		Task->Trigger();
		return Task;
	}

	template<class LAMBDA>
	static std::shared_ptr<TFTask> Launch(Name&& TaskName, LAMBDA&& Lambda, std::vector<TFTask*>&& PrerequisiteTasks, ETFTaskThread Thread = ETFTaskThread::WorkerThread, ETFTaskPriority Priority = ETFTaskPriority::Normal)
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
	static TFTaskEventPtr ParallelFor(Iterator&& Begin, Iterator&& End, LAMBDA&& Lambda, ETFTaskThread Thread = ETFTaskThread::WorkerThread, ETFTaskPriority Priority = ETFTaskPriority::Normal)
	{
		assert(Thread < ETFTaskThread::Num);

		tf::Taskflow TFTaskFlow;
		TFTaskFlow.for_each(Begin, End, std::forward<LAMBDA>(Lambda));
		return DispatchTaskFlow(std::move(TFTaskFlow), Thread, Priority);
	}

	template<class Iterator, class LAMBDA>
	static TFTaskEventPtr ParallelSort(Iterator&& Begin, Iterator&& End, LAMBDA&& Lambda, ETFTaskThread Thread = ETFTaskThread::WorkerThread, ETFTaskPriority Priority = ETFTaskPriority::Normal)
	{
		assert(Thread < ETFTaskThread::Num);

		tf::Taskflow TFTaskFlow;
		TFTaskFlow.sort(Begin, End, std::forward<LAMBDA>(Lambda));
		return DispatchTaskFlow(std::move(TFTaskFlow), Thread, Priority);
	}
protected:
	static TFTaskEventPtr DispatchTaskFlow(tf::Taskflow&&, ETFTaskThread Thread, ETFTaskPriority Priority);

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
	ETFTaskThread m_Thread = ETFTaskThread::WorkerThread;
	ETFTaskPriority m_Priority = ETFTaskPriority::Normal;

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

DECLARE_LOGGER_CATEGORY(LogTaskFlow);