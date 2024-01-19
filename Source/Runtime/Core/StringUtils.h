#pragma once

#include "Runtime/Core/Definitions.h"

namespace StringUtils
{
	void ToLower(std::string& Str);

	std::string Lowercase(const std::string& Str);

	void ToUpper(std::string& Str);

	std::string Uppercase(const std::string& Str);

	void Replace(std::string& Str, const std::string& Src, const std::string& Dst);

	std::string Replace(const std::string& Str, const std::string& Src, const std::string& Dst);

	bool8_t StartsWidth(const std::string& Str, const char* Src);

	bool8_t EndsWidth(const std::string& Str, const char* Src);

	bool8_t Contains(const std::string& Str, const char* Src);

	std::vector<std::string> Split(const std::string& Str, const char* Token);

	std::string Format(const char* Fmt, ...);

	std::wstring ToWide(const std::string& Str);

	std::wstring ToWide(const char* Str);

	std::string ToMultiByte(const std::wstring& Str);

	std::string ToMultiByte(const wchar_t* Str);

	int32_t ToHexDigit(char C);

	uint32_t ToHex(const char* Str);

	template<class StringTypeLeft, class StringTypeRight>
	bool8_t Compare(const StringTypeLeft& Left, const StringTypeRight& Right, bool8_t CaseSensitive = true)
	{
		std::string_view LeftView(Left);
		std::string_view RightView(Right);

		return CaseSensitive ? _stricmp(LeftView.data(), RightView.data()) == 0 : LeftView == RightView;
	}
};


