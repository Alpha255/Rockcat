#pragma once

#include "Runtime/Core/StringUtils.h"

struct Guid
{
	enum class EFormat
	{
		Digits,
		DigitsWithHyphens
	};

	static const char8_t Hyphen = '-';

	uint32_t A = 0u;
	uint32_t B = 0u;
	uint32_t C = 0u;
	uint32_t D = 0u;

	friend bool8_t operator==(const Guid& Left, const Guid& Right)
	{
		return ((Left.A ^ Right.A) | (Left.B ^ Right.B) | (Left.C ^ Right.B) | (Left.D ^ Right.D)) == 0;
	}

	friend bool8_t operator!=(const Guid& Left, const Guid& Right)
	{
		return ((Left.A ^ Right.A) | (Left.B ^ Right.B) | (Left.C ^ Right.B) | (Left.D ^ Right.D)) != 0;
	}

	bool8_t IsValid() const
	{
		return (A | B | C | D) != 0;
	}

	std::string ToString(EFormat Format = EFormat::Digits) const
	{
		if (Format == EFormat::DigitsWithHyphens)
		{
			return StringUtils::Format("%08X-%04X-%04X-%04X-%04X%08X", A, B >> 16, B & 0xFFFF, C >> 16, C & 0xFFFF, D);
		}

		return StringUtils::Format("%08X%08X%08X%08X", A, B, C, D);
	}

	static Guid FromString(const std::string& GuidStr)
	{
		Guid ret;

		std::string Normalized;
		if (GuidStr.length() == 32u)
		{
			Normalized = GuidStr;
		}
		else if (GuidStr.length() == 36u)
		{
			if (GuidStr[8] != Hyphen ||
				GuidStr[13] != Hyphen ||
				GuidStr[18] != Hyphen ||
				GuidStr[23] != Hyphen)
			{
				return ret;
			}

			Normalized += GuidStr.substr(0u, 8u);
			Normalized += GuidStr.substr(9u, 4u);
			Normalized += GuidStr.substr(14u, 4u);
			Normalized += GuidStr.substr(19u, 4u);
			Normalized += GuidStr.substr(24u, 12u);
		}

		for (uint32_t i = 0u; i < Normalized.length(); ++i)
		{
			if (std::isxdigit(static_cast<uchar8_t>(Normalized[i])) == 0)
			{
				return ret;
			}
		}

		ret = Guid(
			StringUtils::ToHex(Normalized.substr(0u, 8u).c_str()),
			StringUtils::ToHex(Normalized.substr(8u, 8u).c_str()),
			StringUtils::ToHex(Normalized.substr(16u, 8u).c_str()),
			StringUtils::ToHex(Normalized.substr(24u, 8u).c_str()));

		return ret;
	}

	static Guid Create();

	Guid() = default;

	Guid(uint32_t PA, uint32_t PB, uint32_t PC, uint32_t PD)
		: A(PA)
		, B(PB)
		, C(PC)
		, D(PD)
	{
	}

	Guid(const char8_t* GuidStr)
	{
		*this = FromString(GuidStr);
	}

	Guid(const std::string& GuidStr)
	{
		*this = FromString(GuidStr);
	}

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
};

