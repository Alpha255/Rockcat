#pragma once

#include "Core/String.h"
#include "Core/Cereal.h"

class Name
{
public:
	Name() = default;

	Name(const char* Value)
		: m_ValueView(Value)
	{
	}

	Name(std::string_view Value)
		: m_ValueView(Value)
	{
	}

	Name(const string& Value)
		: m_Value(Value)
		, m_ValueView(m_Value)
	{
	}

	Name(string&& Value) noexcept
		: m_Value(std::move(Value))
		, m_ValueView(m_Value)
	{
	}

	Name(const Name& Other)
		: m_Value(Other.m_Value)
		, m_ValueView(Other.m_ValueView)
	{
	}

	Name(Name&& Other) noexcept
		: m_Value(std::move(Other.m_Value))
		, m_ValueView(std::move(Other.m_ValueView))
	{
	}

	bool operator==(const Name& Other) const
	{
		return _stricmp(m_ValueView.data(), Other.m_ValueView.data()) == 0;
	}

	inline bool operator!=(const Name& Other) const
	{
		return _stricmp(m_ValueView.data(), Other.m_ValueView.data()) != 0;
	}

	inline Name& operator=(const Name& Other)
	{
		m_Value = Other.m_Value;
		m_ValueView = Other.m_ValueView;
		return *this;
	}

	inline Name& operator=(Name&& Other) noexcept
	{
		m_Value = std::move(Other.m_Value);
		m_ValueView = std::move(Other.m_ValueView);
		return *this;
	}

	inline std::string_view Get() const { return m_ValueView; }

	inline void Set(std::string_view Value) { m_ValueView = Value; }

	inline void Set(const char* Value) { m_ValueView = Value; }

	inline void Set(const string& Value) 
	{
		m_Value = Value;
		m_ValueView = m_Value;
	}

	inline void Set(string&& Value) 
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
	string m_Value;
	std::string_view m_ValueView;
};

namespace std
{
	template<>
	struct hash<Name>
	{
		size_t operator()(const Name& InName) const noexcept
		{
			auto lowercaseName = ::string(InName.Get()).lowercase();
			return hash<std::string>{}(lowercaseName);
		}
	};
}
