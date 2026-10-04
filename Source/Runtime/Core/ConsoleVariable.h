#pragma once

#include "Core/Singleton.h"
#include "Core/Name.h"

#include <charconv>
#include <mutex>
#include <type_traits>

template<class T>
struct TCVarParser
{
	static bool Parse(std::string_view Text, T& Out)
	{
		static_assert(std::is_arithmetic_v<T>, "No parser for this ConsoleVariable type, add a TCVarParser specialization.");

		const auto Result = std::from_chars(Text.data(), Text.data() + Text.size(), Out);
		return Result.ec == std::errc{} && Result.ptr == Text.data() + Text.size();
	}
};

template<>
struct TCVarParser<bool>
{
	static bool Parse(std::string_view Text, bool& Out)
	{
		const string Lower = string(Text).lowercase();

		if (Lower == "true" || Lower == "1")
		{
			Out = true;
			return true;
		}

		if (Lower == "false" || Lower == "0")
		{
			Out = false;
			return true;
		}

		return false;
	}
};

template<>
struct TCVarParser<std::string>
{
	static bool Parse(std::string_view Text, std::string& Out)
	{
		Out.assign(Text.data(), Text.size());
		return true;
	}
};

template<class T, bool = std::is_trivially_copyable_v<T>>
class TCVarStorage
{
public:
	explicit TCVarStorage(const T& DefaultValue)
		: m_Value(DefaultValue)
	{
	}

	void Set(const T& Value)
	{
		std::lock_guard Locker(m_Lock);
		m_Value = Value;
	}

	T Get() const
	{
		std::lock_guard Locker(m_Lock);
		return m_Value;
	}

private:
	mutable std::mutex m_Lock;
	T m_Value;
};

template<class T>
class TCVarStorage<T, true>
{
public:
	explicit TCVarStorage(const T& DefaultValue)
		: m_Value(DefaultValue)
	{
	}

	void Set(const T& Value) { m_Value.store(Value, std::memory_order_relaxed); }
	T Get() const { return m_Value.load(std::memory_order_relaxed); }

private:
	std::atomic<T> m_Value;
};

class ConsoleVariableManager : public Singleton<ConsoleVariableManager>
{
public:
	void RegisterConsoleVariable(class IConsoleVariable* CVar);

	class IConsoleVariable* FindConsoleVariable(const Name& VarName) const;
private:
	Name GetCategory(IConsoleVariable* CVar) const;

	std::unordered_map<Name, std::unordered_map<Name, class IConsoleVariable*>> m_Variables;
};

class IConsoleVariable
{
public:
	IConsoleVariable(const char* Name, const char* Description);
	virtual ~IConsoleVariable() = default;

	const Name& GetName() const { return m_Name; }
	std::string_view GetDescription() const { return m_Description; }

	virtual bool IsBool() const { return false; }
	virtual bool IsInt() const { return false; }
	virtual bool IsUInt() const { return false; }
	virtual bool IsFloat() const { return false; }
	virtual bool IsString() const { return false; }

	bool SetFromString(std::string_view Command);
protected:
	virtual bool SetValue(std::string_view Value) = 0;

private:
	Name m_Name;
	string m_Description;
};

template<class T>
class ConsoleVariable : public IConsoleVariable
{
public:
	ConsoleVariable(const char* Name, const char* Description, const T& DefaultValue)
		: IConsoleVariable(Name, Description)
		, m_Value(DefaultValue)
	{
	}

	void Set(const T& Value) { m_Value.Set(Value); }
	T Get() const { return m_Value.Get(); }

	bool IsBool() const override { return std::is_same_v<T, bool>; }
	bool IsInt() const override { return std::is_same_v<T, int32_t>; }
	bool IsUInt() const override { return std::is_same_v<T, uint32_t>; }
	bool IsFloat() const override { return std::is_same_v<T, float>; }
	bool IsString() const override { return std::is_same_v<T, std::string>; }

protected:
	bool SetValue(std::string_view Value) override
	{
		T Parsed{};
		if (!TCVarParser<T>::Parse(Value, Parsed))
		{
			return false;
		}

		m_Value.Set(Parsed);
		return true;
	}

private:
	TCVarStorage<T> m_Value;
};
