#pragma once

#include "Core/Math/Vector4.h"

NAMESPACE_START(Math)

class Sphere
{
public:
	Sphere() = default;

	Sphere(const Vector3& Center, float Radius)
		: m_Center(Center)
		, m_Radius(Radius)
	{
	}

	const Vector3& GetCenter() const { return m_Center; }
	float GetRadius() const { return m_Radius; }
protected:
private:
	Vector3 m_Center;
	float m_Radius;
};

NAMESPACE_END(Math)
