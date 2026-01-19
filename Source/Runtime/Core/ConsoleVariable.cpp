#include "Core/ConsoleVariable.h" 
#include "Services/SpdLogService.h"

void ConsoleVariableManager::RegisterConsoleVariable(IConsoleVariable* CVar)
{
	assert(CVar);

	std::string Cagetory = GetCategory(CVar);
	auto& VariableGroup = m_Variables[Cagetory];

	if (VariableGroup.find(CVar->GetName()) != VariableGroup.end())
	{
		LOG_WARNING("Console variable '{}' is already registered.", CVar->GetName());
		return;
	}

	VariableGroup.emplace(CVar->GetName(), CVar);
}

IConsoleVariable* ConsoleVariableManager::FindConsoleVariable(std::string_view Name) const
{
	for (const auto& [Category, VariableGroup] : m_Variables)
	{
		auto It = VariableGroup.find(Name);
		if (It != VariableGroup.end())
		{
			return It->second;
		}
	}

	return nullptr;
}

bool IConsoleVariable::SetFromString(std::string_view Command)
{
	return false;
}

std::string ConsoleVariableManager::GetCategory(IConsoleVariable* CVar) const
{
	return std::string();
}
