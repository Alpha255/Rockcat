#pragma once

#include "Core/StringUtils.h"

class Guid
{
public:
	enum class EFormat
	{
		Digits,
		DigitsWithHyphens
	};

	static const char Hyphen = '-';

	friend bool operator==(const Guid& Left, const Guid& Right) { return ((Left.m_A ^ Right.m_A) | (Left.m_B ^ Right.m_B) | (Left.m_C ^ Right.m_B) | (Left.m_D ^ Right.m_D)) == 0; }
	friend bool operator!=(const Guid& Left, const Guid& Right) { return ((Left.m_A ^ Right.m_A) | (Left.m_B ^ Right.m_B) | (Left.m_C ^ Right.m_B) | (Left.m_D ^ Right.m_D)) != 0; }

	inline bool IsValid() const { return (m_A | m_B | m_C | m_D) != 0; }

	std::string ToString(EFormat Format = EFormat::Digits) const
	{
		if (Format == EFormat::DigitsWithHyphens)
		{
			return String::Format("%08X-%04X-%04X-%04X-%04X%08X", m_A, m_B >> 16, m_B & 0xFFFF, m_C >> 16, m_C & 0xFFFF, m_D);
		}

		return String::Format("%08X%08X%08X%08X", m_A, m_B, m_C, m_D);
	}

	static Guid FromString(std::string_view Str)
	{
		Guid ret;

		std::string Normalized;
		if (Str.length() == 32u)
		{
			Normalized = Str.data();
		}
		else if (Str.length() == 36u)
		{
			if (Str[8] != Hyphen ||
				Str[13] != Hyphen ||
				Str[18] != Hyphen ||
				Str[23] != Hyphen)
			{
				return ret;
			}

			Normalized += Str.substr(0u, 8u);
			Normalized += Str.substr(9u, 4u);
			Normalized += Str.substr(14u, 4u);
			Normalized += Str.substr(19u, 4u);
			Normalized += Str.substr(24u, 12u);
		}

		for (uint32_t i = 0u; i < Normalized.length(); ++i)
		{
			if (std::isxdigit(static_cast<char>(Normalized[i])) == 0)
			{
				return ret;
			}
		}

		ret = Guid(
			String::ToHex(Normalized.substr(0u, 8u).c_str()),
			String::ToHex(Normalized.substr(8u, 8u).c_str()),
			String::ToHex(Normalized.substr(16u, 8u).c_str()),
			String::ToHex(Normalized.substr(24u, 8u).c_str()));

		return ret;
	}

	static Guid Create();

	Guid() = default;

	Guid(uint32_t A, uint32_t B, uint32_t C, uint32_t D)
		: m_A(A)
		, m_B(B)
		, m_C(C)
		, m_D(D)
	{
	}

	Guid(std::string_view Str) { *this = FromString(Str); }

	template<class Archive>
	void save(Archive& Ar) const
	{
		std::string Value = ToString();
		Ar(
			Value
		);
	}

	template<class Archive>
	void load(Archive& Ar)
	{
		std::string Value;
		Ar(
			Value
		);

		*this = FromString(Value);
	}
private:
	uint32_t m_A = 0u;
	uint32_t m_B = 0u;
	uint32_t m_C = 0u;
	uint32_t m_D = 0u;
};

