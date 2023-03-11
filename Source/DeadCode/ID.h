#pragma once

#include "Core/Definitions.h"

NAMESPACE_START(Gear)

class UniqueID
{
public:
	UniqueID(const char8_t* Name)
		: m_Name(Name)
	{
	}

	UniqueID(std::string&& Name)
		: m_Name(std::move(Name))
	{
	}
protected:
private:
	uint32_t m_ID = std::numeric_limits<uint32_t>().max();
	std::string m_Name;
};

NAMESPACE_END(Gear)