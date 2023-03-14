#pragma once

#include "Runtime/Core/Definitions.h"

NAMESPACE_START(Gear)

template<class TObject, class TIndex>
class ObjectID
{
public:
	static const TIndex NullIndex = std::numeric_limits<TIndex>().max();
	static_assert(std::is_integral_v<TIndex>, "The index type must be integral");

	using IndexType = TIndex;

	ObjectID() = default;

	ObjectID(const TIndex Index)
		: m_Index(Index)
	{
		assert(m_Index < NullIndex);
	}

	TIndex GetIndex() const
	{
		assert(IsValid());
		return m_Index;
	}

	operator bool8_t() const { return IsValid(); }

	bool8_t IsValid() const { return m_Index != NullIndex; }

	bool8_t operator==(const ObjectID& Other) const { return m_Index == Other.m_Index; };

	bool8_t operator!=(const ObjectID& Other) const { return m_Index != Other.m_Index; };

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
	void serializie(Archive& Ar)
	{
		Ar(
			CEREAL_NVP(m_MaxIndex),
			CEREAL_NVP(m_FreeIndices)
		)
	}
protected:
private:
	TIndex m_MaxIndex = 0;
	std::queue<TIndex> m_FreeIndices;
};

NAMESPACE_END(Gear)
