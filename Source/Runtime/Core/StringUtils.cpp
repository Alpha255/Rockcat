#include "Runtime/Core/StringUtils.h"
#if defined(PLATFORM_WIN32)
	#include <Windows.h>
	#include <windowsx.h>
	#include <shlobj.h>
#else
	#error Unknown platform!
#endif

namespace StringUtils
{
	void ToLower(std::string& Str)
	{
		std::transform(Str.begin(), Str.end(), Str.begin(),
			[](char C)
		{
			return static_cast<char>(std::tolower(static_cast<int32_t>(C)));
		});
	}

	std::string Lowercase(const std::string& Str)
	{
		std::string Temp(Str);
		ToLower(Temp);
		return Temp;
	}

	void ToUpper(std::string& Str)
	{
		std::transform(Str.begin(), Str.end(), Str.begin(),
			[](char C)
		{
			return static_cast<char>(std::toupper(static_cast<int32_t>(C)));
		});
	}

	std::string Uppercase(const std::string& Str)
	{
		std::string Temp(Str);
		ToUpper(Temp);
		return Temp;
	}

	void Replace(std::string& Str, const std::string& Src, const std::string& Dst)
	{
		size_t Index = 0u;
		while (Index < Str.length())
		{
			if (_strnicmp(Str.c_str() + Index, Src.c_str(), Src.length()) == 0)
			{
				Str.replace(Index, Src.length(), Dst);
				Index += Dst.length();
			}
			else
			{
				++Index;
			}
		}
	}

	std::string Replace(const std::string& Str, const std::string& Src, const std::string& Dst)
	{
		std::string Ret(Str);
		Replace(Ret, Src, Dst);
		return Ret;
	}

	bool8_t StartsWidth(const std::string& Str, const char* Src)
	{
		return strncmp(Str.data(), Src, strlen(Src)) == 0;
	}

	bool8_t EndsWidth(const std::string& Str, const char* Src)
	{
		return strcmp(Str.data() + (Str.length() - strlen(Src)), Src) == 0;
	}

	bool8_t Contains(const std::string& Str, const char* Src)
	{
		return Str.find(Src, 0u) != std::string::npos;
	}

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

	std::string Format(const char* Fmt, ...)
	{
		if (Fmt)
		{
			va_list Args = nullptr;
			va_start(Args, Fmt);
			size_t Size = (size_t)_vscprintf(Fmt, Args) + 1u;
			std::unique_ptr<char[]> Ret = std::make_unique<char[]>(Size);
			_vsnprintf_s(Ret.get(), Size, Size, Fmt, Args);
			va_end(Args);
			return std::string(Ret.get());
		}

		return std::string();
	}

	std::wstring ToWide(const std::string& Str)
	{
		return std::wstring(Str.cbegin(), Str.cend());
	}

	std::wstring ToWide(const char* Str)
	{
		std::string Temp(Str);
		return std::wstring(Temp.cbegin(), Temp.cend());
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

