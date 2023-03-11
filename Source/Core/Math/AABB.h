#pragma once

#include "Core/Math/Vector4.h"

NAMESPACE_START(Gear)
NAMESPACE_START(Math)

class AABB
{
public:
	AABB()
		: m_Min(0.0f, 0.0f, 0.0f)
		, m_Max(1.0f, 1.0f, 1.0f)
	{
	}

	AABB(const Vector3& Min, const Vector3& Max)
		: m_Min(Min)
		, m_Max(Max)
	{
	}

	inline Vector3 Min() const
	{
		return m_Min;
	}

	inline Vector3 Max() const
	{
		return m_Max;
	}

	inline Vector3 Center() const
	{
		return (m_Max + m_Min) * 0.5f;
	}

	inline Vector3 Extents() const
	{
		return (m_Max - m_Min) * 0.5f;
	}

	inline static AABB CreateFromVertices(const Vector3* Data, uint32_t Count)
	{
		std::vector<Vector3> Vertices(Count);
		VERIFY(memcpy_s(Vertices.data(), sizeof(Vector3) * Count, Data, sizeof(Vector3) * Count) == 0);
		return CreateFromVertices(Vertices);
	}
	static AABB CreateFromVertices(const std::vector<Vector3>& Vertices);

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_NVP(m_Min),
			CEREAL_NVP(m_Max)
		);
	}
protected:
private:
	Vector3 m_Min;
	Vector3 m_Max;
};

NAMESPACE_END(Math)
NAMESPACE_END(Gear)
