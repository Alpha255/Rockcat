#pragma once

#include "Runtime/Core/Definitions.h"

#pragma warning(disable:4456)
#include <Submodules/taskflow/taskflow/core/task.hpp>
#pragma warning(default:4456)

class Task
{
public:
	enum class ETaskType
	{
		General,
		Graphics,
		ShaderCompile
	};

	enum EPriority
	{
		High = tf::TaskPriority::HIGH,
		Normal = tf::TaskPriority::NORMAL,
		Low = tf::TaskPriority::LOW,
	};

	Task(const char8_t* Name, ETaskType Type, EPriority Priority = EPriority::Normal)
		: m_Name(Name)
		, m_Type(Type)
		, m_Priority(Priority)
	{
	}

	const char8_t* GetName() const { return m_Name.data(); }
	ETaskType GetType() const { return m_Type; }
	EPriority GetPriority() const { return m_Priority; }

	virtual void DoTask() {}
private:
	std::string_view m_Name;
	ETaskType m_Type;
	EPriority m_Priority;
};