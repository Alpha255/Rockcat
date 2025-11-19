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
class ShaderDefineContainer
{
public:
	using Type = ShaderDefineContainer<T...>;
	static constexpr size_t Num = 1u;
};

template<class First, class... Rest>
class ShaderDefineContainer<First, Rest...>
{
public:
	using Type = ShaderDefineContainer<First, Rest...>;
	using Next = ShaderDefineContainer<Rest...>;

	static constexpr size_t Num = Next::Num * First::Num;
};
