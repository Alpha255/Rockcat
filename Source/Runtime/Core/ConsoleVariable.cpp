#include "Core/ConsoleVariable.h"
#include "Core/SpdLogging.h"

void ConsoleVariableManager::RegisterConsoleVariable(IConsoleVariable* CVar)
{
	assert(CVar);

	const Name Category = GetCategory(CVar);
	auto& VariableGroup = m_Variables[Category];

	if (VariableGroup.find(CVar->GetName()) != VariableGroup.end())
	{
		LOG_WARNING(LogDefault, "Console variable '{}' is already registered.", CVar->GetName().Get());
		return;
	}

	VariableGroup.emplace(CVar->GetName(), CVar);
}

IConsoleVariable::IConsoleVariable(const char* Name, const char* Description)
	: m_Name(string(Name))
	, m_Description(Description)
{
	ConsoleVariableManager::Get().RegisterConsoleVariable(this);
}

IConsoleVariable* ConsoleVariableManager::FindConsoleVariable(const Name& VarName) const
{
	for (const auto& [Category, VariableGroup] : m_Variables)
	{
		auto It = VariableGroup.find(VarName);
		if (It != VariableGroup.end())
		{
			return It->second;
		}
	}

	return nullptr;
}

bool IConsoleVariable::SetFromString(std::string_view Command)
{
	const string Trimmed = string(Command).stripped(" ");
	const std::string_view VarName = m_Name.Get();

	if (!Trimmed.starts_with(VarName, ESearchCase::IgnoreCase))
	{
		return false;
	}

	const std::string_view Remainder = std::string_view(Trimmed).substr(VarName.length());

	if (!Remainder.empty() && Remainder.front() != ' ' && Remainder.front() != '\t' && Remainder.front() != '=')
	{
		return false;
	}

	string Value = string(Remainder).stripped(" ");

	if (!Value.empty() && Value.front() == '=')
	{
		Value = string(Value.substr(1u)).stripped(" ");
	}

	if (Value.empty())
	{
		return false;
	}

	return SetValue(Value);
}

Name ConsoleVariableManager::GetCategory(IConsoleVariable* CVar) const
{
	assert(CVar);

	const std::string_view VarName = CVar->GetName().Get();
	const size_t Pos = VarName.find('.');

	if (Pos != std::string_view::npos)
	{
		return Name(VarName.substr(0u, Pos));
	}

	return Name("Common");
}
