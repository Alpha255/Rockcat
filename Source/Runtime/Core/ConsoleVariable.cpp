#include "Core/ConsoleVariable.h" 
#include "Core/StringUtils.h"
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

IConsoleVariable::IConsoleVariable(const char* Name, const char* Description)
	: m_Name(Name)
	, m_Description(Description)
{
	ConsoleVariableManager::Get().RegisterConsoleVariable(this);
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
	auto Name = GetName();
	auto Pos = Command.find(Name);
	if (Pos != std::string_view::npos)
	{
		auto Value = String::Lowercase(std::string(Command.substr(Name.length() + 1u)));

		if (IsBool())
		{
			if (Value == "true")
			{
				static_cast<ConsoleVariable<bool>*>(this)->Set(true);
			}
			else if (Value == "false")
			{
				static_cast<ConsoleVariable<bool>*>(this)->Set(false);
			}
			else
			{
				static_cast<ConsoleVariable<bool>*>(this)->Set(std::stoi(Value.c_str()) > 0);
			}
		}
		else if (IsInt())
		{
			static_cast<ConsoleVariable<int32_t>*>(this)->Set(std::stoi(Value.c_str()));
		}
		else if (IsUInt())
		{
			static_cast<ConsoleVariable<uint32_t>*>(this)->Set(std::stoul(Value.c_str()));
		}
		else if (IsFloat())
		{
			static_cast<ConsoleVariable<float>*>(this)->Set(std::stof(Value.c_str()));
		}
		else if (IsString())
		{
			//static_cast<ConsoleVariable<std::string>*>(this)->Set(Value);
		}
	}

	return false;
}

std::string ConsoleVariableManager::GetCategory(IConsoleVariable* CVar) const
{
	auto Pos = CVar->GetName().find('.');
	if (Pos != std::string_view::npos)
	{
		return std::string(CVar->GetName().substr(0, Pos));
	}

	return std::string("common");
}
