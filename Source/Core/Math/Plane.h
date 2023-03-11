#pragma once

#include "Core/Math/Vector4.h"

NAMESPACE_START(Gear)
NAMESPACE_START(Math)

class Plane : public Vector4
{
public:
	Plane()
		: Vector4(0.0f, 1.0f, 0.0f, 0.0f)
	{
	}

	Plane(const Vector3& Normal, float32_t Distance)
		: Vector4(Normal, Distance)
	{
	}

	Plane(const Vector3& V0, const Vector3& V1, const Vector3& V2)
	{
#if USE_SSE
		VECTOR_STORE(4, 
			this, 
			DirectX::XMPlaneFromPoints(
				VECTOR_LOAD(3, &V0),
				VECTOR_LOAD(3, &V1),
				VECTOR_LOAD(3, &V2)));
#else
		Vector3 Edge10 = V0 - V1;
		Vector3 Edge20 = V0 - V2;
		Edge10.Cross(Edge20);

		Vector3 Normal(Edge10);
		Normal.Normalize();

		Plane Ret(Normal, -Normal.dot(V1));
		*this = Ret;
#endif
	}

	Plane(const Vector3& Point, const Vector3& Normal)
	{
#if USE_SSE
		VECTOR_STORE(4, 
			this, 
			DirectX::XMPlaneFromPointNormal(
				VECTOR_LOAD(3, &Point),
				VECTOR_LOAD(3, &Normal)));
#else
		Plane Ret(Normal, -Normal.Dot(Point));
		*this = Ret;
#endif
	}

	inline Vector3 Normal() const
	{
		return Vector3(x, y, z);
	}

	inline float32_t Distance() const
	{
		return w;
	}

	inline void Normalize()
	{
#if USE_SSE
		VECTOR_STORE(4, this, DirectX::XMPlaneNormalize(VECTOR_LOAD(4, this)));
#else
		Vector3 Normal = Normal();
		Normal.Normalize();

		x = Normal.x;
		y = Normal.y;
		z = Normal.z;
		w = w * Normal.ReciprocalLength();
#endif
	}
protected:
private:
};

NAMESPACE_END(Math)
NAMESPACE_END(Gear)
