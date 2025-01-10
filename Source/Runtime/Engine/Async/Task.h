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
	FileWatchThread,
	WorkerThread,
	Num
};

class TaskEvent : public tf::Future<void>
{
public:
	using tf::Future<void>::Future;

	TaskEvent(tf::Future<void>&& Other) noexcept
		: tf::Future<void>(std::forward<tf::Future<void>>(Other))
	{
	}

	inline bool IsDispatched() const { return valid(); }
	inline bool IsCompleted() const { return valid() ? wait_for(std::chrono::milliseconds(0u)) == std::future_status::ready : false; }
};

class Task : public tf::Task
{
public:
	enum class EPriority
	{
		Critical,
		High,
		Normal,
		Low,
	};

	using tf::Task::Task;

	template<class T>
	Task(T&& Name, EPriority Priority = EPriority::Normal) // Without graph
		: tf::Task()
		, m_Priority(Priority)
		, m_Name(std::forward<T>(Name))
	{
	}

	Task(const Task& Other)
		: tf::Task(Other)
		, m_Priority(Other.m_Priority)
		, m_Name(Other.m_Name)
	{
		ResetNodeName();
	}

	Task(Task&& Other) noexcept
		: tf::Task(std::move(static_cast<tf::Task&&>(Other)))
		, m_Priority(Other.m_Priority)
		, m_Name(std::move(Other.m_Name))
	{
		ResetNodeName();
	}

	Task& operator=(const Task& Other)
	{
		m_Priority = Other.m_Priority;
		m_Name = Other.m_Name;
		ResetNodeName();
		return *this;
	}

	const char* GetName() const { return m_Name.c_str(); }

	template<class T>
	Task& SetName(T&& Name)
	{
		m_Name = std::move(std::string(std::forward<T>(Name)));
		ResetNodeName();
		return *this;
	}

	EPriority GetPriority() const { return m_Priority; }
	Task& SetPriority(EPriority Priority) { m_Priority = Priority; return *this; }

	virtual void Execute() = 0;
protected:
	friend class TaskFlow;

	void SetBaseTask(tf::Task&& Task)
	{
		Cast<tf::Task>(*this) = Task;
		ResetNodeName();
	}
private:
	void ResetNodeName()
	{
#if _DEBUG
		if (!empty())
		{
			tf::Task::name(m_Name);
		}
#endif
	}

	EPriority m_Priority = EPriority::Normal;
	std::string m_Name;
};