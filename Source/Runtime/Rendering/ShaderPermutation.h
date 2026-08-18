#pragma once

#include "Core/Definitions.h"

class ShaderDefine
{
public:
	ShaderDefine(const char* Name)
		: m_Name(Name)
	{
	}

private:
	std::string_view m_Name;
};

class ShaderDefineBool
{
public:
	using Type = bool;
	static constexpr size_t Num = 2u;
};

template<int32_t... T>
class ShaderDefineInt32
{
public:
	using Type = int32_t;
	static constexpr size_t Num = 0u;
};

template<int32_t First, int32_t... Rest>
class ShaderDefineInt32<First, Rest...>
{
public:
	using Type = int32_t;
	static constexpr size_t Num = ShaderDefineInt32<Rest...>::Num + 1u;
};

template<class... T>
class ShaderVariants
{
public:
	using Type = ShaderVariants<T...>;
	static constexpr size_t Num = 1u;
};

template<class First, class... Rest>
class ShaderVariants<First, Rest...>
{
public:
	using Type = ShaderVariants<First, Rest...>;
	using Next = ShaderVariants<Rest...>;

	static constexpr size_t Num = Next::Num * First::Num;

	template<class Variable>
	void Set(const auto& Value)
	{
		if constexpr (std::is_same_v<Variable, First>)
		{
			m_Value = Value;
		}
		else
		{
			m_Next.Set<Variable>(Value);
		}
	}

	template<class Variable>
	const auto& Get() const
	{
		if constexpr (std::is_same_v<Variable, First>)
		{
			return m_Value;
		}
		else
		{
			return m_Next.Get<Variable>();
		}
	}
private:
	typename First::Type m_Value;
	Next m_Next;
};
