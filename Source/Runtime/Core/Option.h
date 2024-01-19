#pragma once

#include "Runtime/Core/Definitions.h"

template<class T>
class Option
{
public:
	Option(const T& Value, const char* Description)
		: m_Value(Value)
		, m_Description(Description)
	{
	}

	Option& operator=(const T& Value)
	{
		m_Value = Value;
		return *this;
	}

	const T& operator()() const
	{
		return m_Value;
	}
protected:
private:
	T m_Value;
	std::string_view m_Description;
};

template<class T>
class Optional : public std::optional<T>
{
public:
	Optional(const T& Value, const char* Description)
		: std::optional<T>(Value)
		, m_Description(Description)
	{
	}
private:
	std::string_view m_Description;
};

