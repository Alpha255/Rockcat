#pragma once

#include "Core/Definitions.h"

namespace String
{
	std::vector<std::string> Split(const std::string& Str, const char* Token);

#if _HAS_CXX20
	template<class ...Args>
	inline std::string vFormat(const char* Fmt, Args&&... InArgs)
	{
		return std::vformat(Fmt, std::make_format_args(InArgs...));
	}
#endif

	std::string ToMultiByte(const std::wstring& Str);
	std::string ToMultiByte(const wchar_t* Str);

	int32_t ToHexDigit(char C);
	uint32_t ToHex(const char* Str);
};

enum class ESearchCase
{
	CaseSensitive,
	IgnoreCase
};

class string : public std::string
{
public:
	using std::string::string;

	void tolower();
	void toupper();

	string uppercase() const;
	string lowercase() const;

	void replace(std::string_view from, std::string_view to, ESearchCase searchcase = ESearchCase::CaseSensitive);
	string replaced(std::string_view from, std::string_view to, ESearchCase searchcase = ESearchCase::CaseSensitive) const;

	bool starts_with(std::string_view prefix, ESearchCase searchcase = ESearchCase::CaseSensitive) const;
	bool ends_with(std::string_view postfix, ESearchCase searchcase = ESearchCase::CaseSensitive) const;
	bool contains(std::string_view substr, ESearchCase searchcase = ESearchCase::CaseSensitive) const;

	std::vector<string> splits(std::string_view token, ESearchCase searchcase = ESearchCase::CaseSensitive) const;

	std::wstring towide() const;

	bool compare(std::string_view other, ESearchCase searchcase = ESearchCase::CaseSensitive) const;

	static string format(std::string_view format, ...);
};


