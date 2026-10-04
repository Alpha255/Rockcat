#include "Core/String.h"
#include "Misc/PlatformMisc.h"
#include <stdarg.h>

void string::tolower()
{
	std::transform(begin(), end(), begin(), [](value_type c) {
		return static_cast<value_type>(std::tolower(static_cast<unsigned char>(c)));
	});
}

void string::toupper()
{
	std::transform(begin(), end(), begin(), [](value_type c) {
		return static_cast<value_type>(std::toupper(static_cast<unsigned char>(c)));
	});
}

string string::uppercase() const
{
	string ret(*this);
	ret.toupper();
	return ret;
}

string string::lowercase() const
{
	string ret(*this);
	ret.tolower();
	return ret;
}

void string::replace(std::string_view from, std::string_view to, ESearchCase searchcase)
{
	if (from.empty())
	{
		return;
	}

	auto comparefunc = searchcase == ESearchCase::IgnoreCase ? _strnicmp : strncmp;

	size_t index = 0u;
	while (index + from.length() <= length())
	{
		if (comparefunc(c_str() + index, from.data(), from.length()) == 0)
		{
			std::string::replace(index, from.length(), to);
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
	if (prefix.empty())
	{
		return true;
	}

	if (prefix.length() > length())
	{
		return false;
	}

	auto comparefunc = searchcase == ESearchCase::IgnoreCase ? _strnicmp : strncmp;
	return comparefunc(c_str(), prefix.data(), prefix.length()) == 0;
}

bool string::ends_with(std::string_view postfix, ESearchCase searchcase) const
{
	if (postfix.empty())
	{
		return true;
	}

	if (postfix.length() > length())
	{
		return false;
	}

	auto comparefunc = searchcase == ESearchCase::IgnoreCase ? _strnicmp : strncmp;
	return comparefunc(c_str() + (length() - postfix.length()), postfix.data(), postfix.length()) == 0;
}

bool string::contains(std::string_view substr, ESearchCase searchcase) const
{
	if (substr.empty())
	{
		return true;
	}

	if (searchcase == ESearchCase::CaseSensitive)
	{
		return std::string::find(substr) != string::npos;
	}
	else
	{
		size_t index = 0u;
		while (index + substr.length() <= length())
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

void string::strip(std::string_view token, ESearchCase searchcase)
{
	if (token.empty() || token.length() > length())
	{
		return;
	}

	auto comparefunc = searchcase == ESearchCase::IgnoreCase ? _strnicmp : strncmp;

	while (token.length() <= length() && comparefunc(c_str(), token.data(), token.length()) == 0)
	{
		erase(0u, token.length());
	}

	while (token.length() <= length() && comparefunc(c_str() + (length() - token.length()), token.data(), token.length()) == 0)
	{
		erase(length() - token.length(), token.length());
	}
}

string string::stripped(std::string_view token, ESearchCase searchcase) const
{
	string ret(*this);
	ret.strip(token, searchcase);
	return ret;
}

std::vector<string> string::splits(std::string_view token, ESearchCase searchcase) const
{
	std::vector<string> ret;

	if (token.empty())
	{
		return ret;
	}

	auto comparefunc = searchcase == ESearchCase::IgnoreCase ? _strnicmp : strncmp;

	size_t index = 0u, offset = 0u;
	while ((index + token.length()) <= length())
	{
		if (comparefunc(c_str() + index, token.data(), token.length()) == 0)
		{
			auto sub = substr(offset, index - offset);
			if (!sub.empty())
			{
				ret.emplace_back(std::move(sub));
			}

			index += token.length();
			offset = index;
		}
		else
		{
			++index;
		}
	}

	auto sub = substr(offset);
	if (!sub.empty())
	{
		ret.emplace_back(std::move(sub));
	}

	return ret;
}

std::wstring string::to_wide() const
{
	return PlatformMisc::Utf8ToWide(*this);
}

string string::from_wide(std::wstring_view str)
{
	return PlatformMisc::WideToUtf8(str);
}

bool string::compare(std::string_view other, ESearchCase searchcase) const
{
	if (other.length() != length())
	{
		return false;
	}

	if (empty())
	{
		return true;
	}

	auto comparefunc = searchcase == ESearchCase::IgnoreCase ? _strnicmp : strncmp;
	return comparefunc(c_str(), other.data(), length()) == 0;
}

string string::printf(std::string_view format, ...)
{
	if (format.empty())
	{
		return string();
	}

	const string formatstr(format);

	va_list args;
	va_start(args, format);

	va_list argsCopy;
	va_copy(argsCopy, args);
	const int required = _vscprintf(formatstr.c_str(), argsCopy);
	va_end(argsCopy);

	if (required < 0)
	{
		va_end(args);
		return string();
	}

	const size_t size = static_cast<size_t>(required) + 1u;
	std::unique_ptr<value_type[]> buffer = std::make_unique<value_type[]>(size);
	_vsnprintf_s(buffer.get(), size, _TRUNCATE, formatstr.c_str(), args);
	va_end(args);

	return string(buffer.get());
}
