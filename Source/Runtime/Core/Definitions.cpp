#include "Core/Definitions.h"

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

void SplitEnumArgs(const char* Args, std::string Names[], uint32_t Max)
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

size_t GetPowerOfTwo(uint32_t Value)
{
	if (Value == 0u)
	{
		return 0u;
	}

	if (!IsPowerOfTwo(Value))
	{
		return 0u;
	}

	size_t Count = 0u;

	while (Value >>= 1u)
	{
		++Count;
	}
	return Count;
}
