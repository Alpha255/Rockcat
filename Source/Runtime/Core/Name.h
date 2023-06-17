#pragma once

#include "Runtime/Core/StringUtils.h"

template<bool CaseSensitive>
class GeneralName : public std::string
{
public:
	using std::string::string;

	constexpr bool8_t operator==(const GeneralName& Other) const
	{
		if (CaseSensitive)
		{
			return this->_Equal(Other);
		}
		else
		{
			return _stricmp(this->c_str(), Other.c_str()) == 0;
		}
	}
};

using CaseSensitiveName = GeneralName<true>;
using CaseInSensitiveName = GeneralName<false>;

namespace std
{
	template<>
	struct hash<CaseSensitiveName>
	{
		size_t operator()(const CaseSensitiveName& Name) const noexcept
		{
			return hash<std::string>::_Do_hash(Name);
		}
	};

	template<>
	struct hash<CaseInSensitiveName>
	{
		size_t operator()(const CaseInSensitiveName& Name) const noexcept
		{
			return hash<std::string>::_Do_hash(Name);
		}
	};
}
