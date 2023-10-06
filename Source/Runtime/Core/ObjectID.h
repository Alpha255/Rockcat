#pragma once

#include "Runtime/Core/Definitions.h"
#include "Runtime/Core/Cereal.h"

template<class TObject, class TIndex = uint32_t>
class ObjectID
{
public:
	static const constexpr TIndex NullIndex = std::numeric_limits<TIndex>().max();
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

	constexpr operator bool8_t() const { return IsValid(); }

	constexpr bool8_t IsValid() const { return m_Index != NullIndex; }

	constexpr bool8_t operator==(const ObjectID& Other) const { return m_Index == Other.m_Index; }

	constexpr bool8_t operator!=(const ObjectID& Other) const { return m_Index != Other.m_Index; }

	constexpr bool8_t operator==(const TIndex Index) const { return m_Index == Index; }

	constexpr bool8_t operator!=(const TIndex Index) const { return m_Index != Index; }

	struct Hasher
	{
		size_t operator()(const ObjectID& Value) const
		{
			return static_cast<size_t>(Value.Index());
		}
	};

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

template<class TObject, class TIndex>
class ObjectIDAllocator
{
public:
	using TID = ObjectID<TObject, TIndex>;

	TID Allocate()
	{
		if (!m_FreeIndices.empty())
		{
			TIndex Index = m_FreeIndices.front();
			m_FreeIndices.pop();
			return TID(Index);
		}

		return TID(m_MaxIndex++);
	}

	void Free(TIndex Index) { m_FreeIndices.push(Index); }

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_NVP(m_MaxIndex),
			CEREAL_NVP(m_FreeIndices)
		);
	}
protected:
private:
	TIndex m_MaxIndex = 0;
	std::queue<TIndex> m_FreeIndices;
};

#define DECLARE_OBJECT_ID(ObjectType, IDType) using ObjectType##ID = ObjectID<class ObjectType, IDType>; using ObjectType##IDAllocator = ObjectIDAllocator<class ObjectType, IDType>;

