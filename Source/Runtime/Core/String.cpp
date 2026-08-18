#include "Core/String.h"
#include <stdarg.h>

namespace String
{
	std::vector<std::string> Split(const std::string& Str, const char* Token)
	{
		std::vector<std::string> Ret;

		std::string Temp(Str);

		size_t Index = Temp.find(Token);

		while (Index != std::string::npos)
		{
			Ret.emplace_back(Temp.substr(0u, Index));
			Temp = Temp.substr(Index + 1u);
			Index = Temp.find(Token);
		}

		if (Temp.length() > 0u)
		{
			Ret.emplace_back(std::move(Temp));
		}

		return Ret;
	}

	std::string ToMultiByte(const std::wstring& Str)
	{
		std::setlocale(LC_CTYPE, "");
		size_t Length = 0ull;
		VERIFY(::wcstombs_s(&Length, nullptr, 0u, Str.c_str(), 0u) == 0);
		std::vector<char> Temp(Length + 1u);
		VERIFY(::wcstombs_s(&Length, Temp.data(), Temp.size(), Str.c_str(), Temp.size()) == 0);
		return std::string(Temp.data());
	}

	std::string ToMultiByte(const wchar_t* Str)
	{
		return ToMultiByte(std::wstring(Str));
	}

	int32_t ToHexDigit(char C)
	{
		int32_t Ret = 0;

		if (C >= '0' && C <= '9')
		{
			Ret = C - '0';
		}
		else if (C >= 'a' && C <= 'f')
		{
			Ret = C + 10 - 'a';
		}
		else if (C >= 'A' && C <= 'F')
		{
			Ret = C + 10 - 'A';
		}
		else
		{
			Ret = 0;
		}

		return Ret;
	}

	uint32_t ToHex(const char* Str)
	{
		uint32_t Ret = 0u;

		while (*Str)
		{
			Ret *= 16;
			Ret += ToHexDigit(*Str++);
		}

		return Ret;
	}
}

void string::tolower()
{
	std::transform(begin(), end(), begin(), [](value_type c) {
		return static_cast<value_type>(std::tolower(static_cast<int32_t>(c)));
	});
}

void string::toupper()
{
	std::transform(begin(), end(), begin(), [](value_type c) {
		return static_cast<value_type>(std::tolower(static_cast<int32_t>(c)));
	});
}

string string::uppercase() const
{
	string ret(*this);
	ret.uppercase();
	return ret;
}

string string::lowercase() const
{
	string ret(*this);
	ret.lowercase();
	return ret;
}

void string::replace(std::string_view from, std::string_view to, ESearchCase searchcase)
{
	auto comparefunc = searchcase == ESearchCase::IgnoreCase ? _strnicmp : strncmp;

	size_t index = 0u;
	while (index < length())
	{
		if (comparefunc(c_str() + index, from.data(), from.length()) == 0)
		{
			std::string::replace(index, from.length(), to.data());
			index += to.length();
		}
		else
		{
			++index;
		}
	}
}

string string::replaced(std::string_view from, std::string_view to, ESearchCase searchcase) const
{
	string ret(*this);
	ret.replace(from, to, searchcase);
	return ret;
}

bool string::starts_with(std::string_view prefix, ESearchCase searchcase) const
{
	auto comparefunc = searchcase == ESearchCase::IgnoreCase ? _strnicmp : strncmp;
	return comparefunc(c_str(), prefix.data(), prefix.length()) == 0;
}

bool string::ends_with(std::string_view postfix, ESearchCase searchcase) const
{
	auto comparefunc = searchcase == ESearchCase::IgnoreCase ? _strcmpi : strcmp;
	return comparefunc(c_str() + (length() - postfix.length()), postfix.data()) == 0;
}

bool string::contains(std::string_view substr, ESearchCase searchcase) const
{
	if (searchcase == ESearchCase::CaseSensitive)
	{
		return std::string::find(substr) != string::npos;
	}
	else
	{
		size_t index = 0u;
		while (index < length())
		{
			if (_strnicmp(c_str() + index, substr.data(), substr.length()) == 0)
			{
				return true;
			}
			else
			{
				++index;
			}
		}
	}

	return false;
}

std::vector<string> string::splits(std::string_view token, ESearchCase searchcase) const
{
	return std::vector<string>();
}

std::wstring string::towide() const
{
	return std::wstring(cbegin(), cend());
}

bool string::compare(std::string_view other, ESearchCase searchcase) const
{
	auto comparefunc = searchcase == ESearchCase::IgnoreCase ? _strcmpi : strcmp;
	return comparefunc(c_str(), other.data()) == 0;
}

string string::format(std::string_view format, ...)
{
	if (!format.empty())
	{
		va_list args = nullptr;
		va_start(args, format.data());
		size_t size = (size_t)_vscprintf(format.data(), args) + 1u;
		std::unique_ptr<value_type[]> buf = std::make_unique<value_type[]>(size);
		_vsnprintf_s(buf.get(), size, size, format.data(), args);
		va_end(args);
		return string(buf.get());
	}

	return string();
}
