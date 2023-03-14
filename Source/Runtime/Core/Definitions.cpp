#include "Runtime/Core/Definitions.h"

NAMESPACE_START(Gear)

std::string TrimEnumString(const std::string& Name)
{
	std::string::const_iterator It = Name.begin();
	while (It != Name.end() && std::isspace(*It))
	{
		It++;
	}

	std::string::const_reverse_iterator ReverseIt = Name.rbegin();
	while (ReverseIt.base() != It && std::isspace(*ReverseIt))
	{
		ReverseIt++;
	}

	return std::string(It, ReverseIt.base());
}

void SplitEnumArgs(const char8_t* Args, std::string Names[], uint32_t Max)
{
	std::stringstream SS(Args);
	std::string SubStr;
	uint32_t Index = 0u;
	while (SS.good() && Index < Max)
	{
		std::getline(SS, SubStr, ',');
		Names[Index] = TrimEnumString(SubStr);
		++Index;
	}
}

size_t PopulationCount(size_t Value)
{
#if 0
	size_t Count = 0u;
	while (Value != 0u)
	{
		Value &= (Value - 1);
		++Count;
	}
	return Count;
#else
	return ::__popcnt64(Value);
#endif
}

NAMESPACE_END(Gear)