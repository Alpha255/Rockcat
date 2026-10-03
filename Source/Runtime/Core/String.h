#pragma once

#include "Core/Definitions.h"

enum class ESearchCase
{
	CaseSensitive,
	IgnoreCase
};

class string : public std::string
{
public:
	using std::string::string;

	string(const std::string& other) : std::string(other) {}
	string(std::string&& other) noexcept : std::string(static_cast<std::string&&>(other)) {}

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

	static string printf(std::string_view format, ...);

	template<class... Args>
	static string format(std::format_string<Args...> fmt, Args&&... args)
	{
		return std::format(fmt, std::forward<Args>(args)...);
	}
};


