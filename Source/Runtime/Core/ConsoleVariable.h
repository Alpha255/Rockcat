#pragma once

#include "Core/Singleton.h"

class ConsoleVariableManager : public Singleton<ConsoleVariableManager>
{
public:
	void RegisterConsoleVariable(class IConsoleVariable* CVar);

	class IConsoleVariable* FindConsoleVariable(std::string_view Name) const;
private:
	std::string GetCategory(IConsoleVariable* CVar) const;

	std::unordered_map<std::string, std::unordered_map<std::string_view, class IConsoleVariable*>> m_Variables;
};

class IConsoleVariable
{
public:
	IConsoleVariable(const char* Name, const char* Description)
		: m_Name(Name)
		, m_Description(Description)
	{
	}

	inline std::string_view GetName() const { return m_Name; }
	inline std::string_view GetDescription() const { return m_Description; }

	inline virtual bool IsBool() const { return false; }
	inline virtual bool IsInt() const { return false; }
	inline virtual bool IsUInt() const { return false; }
	inline virtual bool IsFloat() const { return false; }
	inline virtual bool IsString() const { return false; }

	bool SetFromString(std::string_view Command);
private:
	std::string_view m_Name;
	std::string_view m_Description;
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

	inline void Set(const T& Value) { m_Value.store(Value); }
	inline T Get() const { return m_Value.load(); }

	inline bool IsBool() const override { return std::is_same_v<T, bool>; }
	inline bool IsInt() const override { return std::is_same_v<T, int32_t>; }
	inline bool IsUInt() const override { return std::is_same_v<T, uint32_t>; }
	inline bool IsFloat() const override { return std::is_same_v<T, float>; }
	inline bool IsString() const override { return std::is_same_v<T, std::string> || std::is_same_v<T, const char*>; }
private:
	std::string_view m_Name;
	std::string_view m_Description;

	std::atomic<T> m_Value;
};
