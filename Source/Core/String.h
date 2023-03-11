#pragma once

#include "Core/Definitions.h"

NAMESPACE_START(Gear)

namespace String
{
	void ToLower(std::string& Str);

	std::string Lowercase(const std::string& Str);

	void ToUpper(std::string& Str);

	std::string Uppercase(const std::string& Str);

	void Replace(std::string& Str, const std::string& Src, const std::string& Dst);

	bool8_t StartsWidth(const std::string& Str, const char8_t* Src);

	bool8_t EndsWidth(const std::string& Str, const char8_t* Src);

	bool8_t Contains(const std::string& Str, const char8_t* Src);

	std::vector<std::string> Split(const std::string& Str, const char8_t* Token);

	std::string Format(const char8_t* Fmt, ...);

	std::wstring ToWide(const std::string& Str);

	std::wstring ToWide(const char8_t* Str);

	std::string ToMultiByte(const std::wstring& Str);

	std::string ToMultiByte(const wchar_t* Str);

	int32_t ToHexDigit(char8_t C);

	uint32_t ToHex(const char8_t* Str);
};

NAMESPACE_END(Gear)


