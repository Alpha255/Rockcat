#pragma once

#include "Core/StringUtils.h"
#include "Core/Cereal.h"

template<bool CaseSensitive>
class GenericName
{
public:
	GenericName() = default;

	GenericName(const char* Value)
		: m_ValueView(Value)
	{
	}

	GenericName(const std::string& Value)
		: m_Value(Value)
		, m_ValueView(m_Value)
	{
	}

	GenericName(std::string&& Value) noexcept
		: m_Value(std::move(Value))
		, m_ValueView(m_Value)
	{
	}

	GenericName(const GenericName& Other)
		: m_Value(Other.m_Value)
		, m_ValueView(Other.m_ValueView)
	{
	}

	GenericName(GenericName&& Other) noexcept
		: m_Value(std::move(Other.m_Value))
		, m_ValueView(std::move(Other.m_ValueView))
	{
	}

	bool operator==(const GenericName& Other) const
	{
		if constexpr (CaseSensitive)
		{
			return m_ValueView == Other.m_ValueView;
		}
		else
		{
			return _stricmp(m_ValueView.data(), Other.m_ValueView.data()) == 0;
		}
	}

	inline bool operator!=(const GenericName& Other) const
	{
		return !(*this == Other);
	}

	inline GenericName& operator=(const GenericName& Other)
	{
		m_Value = Other.m_Value;
		m_ValueView = Other.m_ValueView;
		return *this;
	}

	inline GenericName& operator=(GenericName&& Other) noexcept
	{
		m_Value = std::move(Other.m_Value);
		m_ValueView = std::move(Other.m_ValueView);
		return *this;
	}

	inline std::string_view Get() const { return m_ValueView; }

	inline void Set(const char* Value) { m_ValueView = Value; }

	inline void Set(const std::string& Value) 
	{
		m_Value = Value;
		m_ValueView = m_Value;
	}

	inline void Set(std::string&& Value) 
	{
		m_Value = std::move(Value);
		m_ValueView = m_Value;
	}

	template<class Archive>
	void serialize(Archive& Ar)
	{
		if constexpr (Archive::is_saving::value)
		{
			if (m_Value.empty())
			{
				m_Value = m_ValueView;
			}
		}

		Ar(
			CEREAL_NVP(m_Value)
		);

		m_ValueView = m_Value;
	}

private:
	std::string m_Value;
	std::string_view m_ValueView;
};


using FName = GenericName<true>;
using CName = GenericName<false>;

template<bool CaseSensitive>
struct GenericNamedObject
{
	using NameType = GenericName<CaseSensitive>;

	NameType Name;

	inline void SetName(NameType&& InName)
	{
		Name = std::move(InName);
	}

	inline const NameType& GetName() const 
	{
		return Name;
	}

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_NVP(Name)
		);
	}
};

using FNamedObject = GenericNamedObject<true>;
using CNamedObject = GenericNamedObject<false>;

namespace std
{
	template<>
	struct hash<FName>
	{
		size_t operator()(const FName& InName) const noexcept
		{
			return hash<std::string_view>::_Do_hash(InName.Get());
		}
	};

	template<>
	struct hash<CName>
	{
		size_t operator()(const CName& InName) const noexcept
		{
			return hash<std::string_view>::_Do_hash(InName.Get());
		}
	};
}
