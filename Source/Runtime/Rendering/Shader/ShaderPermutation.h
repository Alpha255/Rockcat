#pragma once

#include "Core/String.h"
#include "Rendering/RHI/RHIShader.h"

struct ShaderMetaData
{
	string Name;
	string SourceFilePath;
	string Entry;

	ERHIShaderStage Stage;
};

class ShaderDefines
{
public:
	template<class T>
	inline void SetDefine(const char* Name, T Value) { m_Defines[Name] = (std::stringstream() << Value).str(); }

	template<class T>
	inline void SetDefine(const string& Name, T Value) { m_Defines[Name] = (std::stringstream() << Value).str(); }

	void Merge(const ShaderDefines& Other)
	{
		for (const auto& [Name, Value] : Other.m_Defines)
		{
			SetDefine(Name, Value);
		}
	}

	size_t ComputeHash() const
	{
		size_t Seed = 0u;
		for (const auto& [Name, Value] : m_Defines)
		{
			HashCombine(Seed, std::hash<string>{}(Name), std::hash<string>{}(Value));
		}
		return Seed;
	}

	const std::map<string, string>& GetDefines() const { return m_Defines; }
private:
	std::map<string, string> m_Defines;
};

class ShaderCompileEnvironment : public ShaderDefines
{
public:
	ShaderCompileEnvironment(const ShaderMetaData& MetaData)
		: m_MetaData(MetaData)
	{
	}

	static void BuildCompileArgs();
private:
	const ShaderMetaData& m_MetaData;
};

class ShaderVariantBool
{
public:
	using Type = bool;
	static constexpr size_t Num = 2u;
};

template<class T, size_t Size, int32_t FirstValue = 0>
class ShaderVariantInt
{
public:
	using Type = T;
	static constexpr size_t Num = Size;
};

template<int32_t... T>
class ShaderVariantSparseInt
{
public:
	using Type = int32_t;
	static constexpr size_t Num = 0u;
};

template<int32_t First, int32_t... Rest>
class ShaderVariantSparseInt<First, Rest...>
{
public:
	using Type = int32_t;
	static constexpr size_t Num = ShaderVariantSparseInt<Rest...>::Num + 1u;
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

#define SHADER_VARIANT_BOOL(ShaderVariantClass) \
	class ShaderVariantClass : public ShaderVariantBool \
	{ \
		public: static constexpr const char* Name = ShaderVariantClass; \
	}

#define SHADER_VARIANT_INT(ShaderVariantClass, Num) \
	class ShaderVariantClass : public ShaderVariantInt<int32_t, Num> \
	{ \
		public: static constexpr const char* Name = ShaderVariantClass; \
	}

#define SHADER_VARIANT_UINT(ShaderVariantClass, Num) \
	class ShaderVariantClass : public ShaderVariantInt<uint32_t, Num> \
	{ \
		public: static constexpr const char* Name = ShaderVariantClass; \
	}

#define SHADER_VARIANT_ENUM(ShaderVariantClass, ENUM) \
	class ShaderVariantClass : public ShaderVariantInt<ENUM, static_cast<size_t>(ENUM::Num)> \
	{ \
		public: static constexpr const char* Name = ShaderVariantClass; \
	}

#define SHADER_VARIANT_SPARSE_INT(ShaderVariantClass, ...) \
	class ShaderVariantClass : public ShaderVariantSparseInt<__VA_ARGS__> \
	{ \
		public: static constexpr const char* Name = ShaderVariantClass; \
	}