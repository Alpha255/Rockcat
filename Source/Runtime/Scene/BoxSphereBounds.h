#pragma once

#include "Core/Math/AABB.h"
#include "Core/Math/Sphere.h"

class BoxSphereBounds
{
public:
	BoxSphereBounds() = default;
	BoxSphereBounds(const Math::Vector3& Origin, const Math::Vector3& Extents, const float Radius)
		: m_Origin(Origin)
		, m_Extents(Extents)
		, m_SphereRadius(Radius)
	{
	}
	
	inline const Math::Vector3& GetOrigin() const { return m_Origin; }
	inline const Math::Vector3& GetExtents() const { return m_Extents; }
	inline Math::AABB GetAABB() const { return Math::AABB(m_Origin - m_Extents, m_Origin + m_Extents); }
	inline Math::Sphere GetSphere() const { return Math::Sphere(m_Origin, m_SphereRadius); }

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_NVP(m_Origin),
			CEREAL_NVP(m_Extents),
			CEREAL_NVP(m_SphereRadius)
		);
	}
private:
	Math::Vector3 m_Origin;
	Math::Vector3 m_Extents;

	float m_SphereRadius = 0.0f;
};
