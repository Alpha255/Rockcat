#pragma once

#include "Core/Definitions.h"
#include "Core/Cereal.h"

template<class TObject, class TIndex = uint32_t>
class ObjectID
{
public:
	static const constexpr TIndex NullIndex = std::numeric_limits<TIndex>::max();
	static_assert(std::is_unsigned<TIndex>::value, "The index type must be unsigned integral");

	using IndexType = TIndex;

	constexpr ObjectID() = default;

	constexpr ObjectID(TIndex Index) noexcept
		: m_Index(Index)
	{
	}

	constexpr ObjectID(const ObjectID& Other) noexcept
		: m_Index(Other.m_Index)
	{
	}

	constexpr ObjectID(ObjectID&& Other) noexcept
		: m_Index(Other.m_Index)
	{
		Other.m_Index = NullIndex;
	}

	constexpr ObjectID& operator=(const TIndex Index) { m_Index = Index; return *this; }
	constexpr ObjectID& operator=(const ObjectID& Other) { m_Index = Other.m_Index; return *this; }
	constexpr ObjectID& operator+=(const TIndex Value) { m_Index += Value; return *this; }
	constexpr ObjectID& operator-=(const TIndex Value) { m_Index -= Value; return *this; }

	friend ObjectID operator+(const ObjectID& Src, const TIndex Value) { return ObjectID(Src.GetIndex() + Value); }
	friend ObjectID operator-(const ObjectID& Src, const TIndex Value) { return ObjectID(Src.GetIndex() - Value); }

	constexpr TIndex GetIndex() const { assert(IsValid()); return m_Index; }

	constexpr operator bool() const { return IsValid(); }
	constexpr bool IsValid() const { return m_Index != NullIndex; }

	constexpr bool operator==(const ObjectID& Other) const { return m_Index == Other.m_Index; }
	constexpr bool operator!=(const ObjectID& Other) const { return m_Index != Other.m_Index; }

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_NVP(m_Index)
		);
	}
private:
	TIndex m_Index = NullIndex;
};

namespace std
{
	template<class TObject, class TIndex>
	struct hash<ObjectID<TObject, TIndex>>
	{
		size_t operator()(const ObjectID<TObject, TIndex>& ID) const
		{
			return std::hash<TIndex>::_Do_hash(ID.GetIndex());
		}
	};
}

