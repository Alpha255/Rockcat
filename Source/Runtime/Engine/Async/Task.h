#pragma once

#include "Core/Definitions.h"

#pragma warning(push)
#pragma warning(disable:4456 4244 4127 4267 4324)
#include <Submodules/taskflow/taskflow/taskflow.hpp>
#include <Submodules/taskflow/taskflow/core/task.hpp>
#include <Submodules/taskflow/taskflow/algorithm/for_each.hpp>
#include <Submodules/taskflow/taskflow/algorithm/sort.hpp>
#pragma warning(pop)

namespace tf
{
	enum class EThread
	{
		GameThread,
		RenderThread,
		FileWatchThread,
		WorkerThread,
		Num
	};

	enum class EPriority
	{
		Critical,
		High,
		Normal,
		Low,
	};

	class ThreadTask : public Task
	{
	public:
		using Task::Task;

		ThreadTask(std::string Name, EPriority Priority = EPriority::Normal) // Without graph
			: m_Priority(Priority)
			, m_Name(Name)
		{
		}

		ThreadTask(const ThreadTask& Other)
			: Task(Other)
			, m_Priority(Other.m_Priority)
			, m_Name(Other.m_Name)
		{
			Task::name(std::string(Other.GetName()));
		}

		ThreadTask(const Task& Other)
			: Task(Other)
		{
			Task::name(std::string(GetName()));
		}

		const char* GetName() const { return m_Name.c_str(); }
		ThreadTask& SetName(const char* Name) 
		{ 
			m_Name = Name;
			Task::name(std::string(Name)); 
			return *this; 
		}

		EPriority GetPriority() const { return m_Priority; }
		ThreadTask& SetPriority(EPriority Priority) { m_Priority = Priority; return *this; }

		virtual void Execute() = 0;
	private:
		EPriority m_Priority = EPriority::Normal;
		std::string m_Name;
	};
}