#pragma once

#include "Core/Math/Vector2.h"

NAMESPACE_START(Math)

class Vector3 : public Float3
{
public:
	inline Vector3()
		: Float3(0.0f, 0.0f, 0.0f)
	{
	}

	inline Vector3(float X, float Y, float Z)
		: Float3(X, Y, Z)
	{
	}

	inline Vector3(float Value)
		: Float3(Value, Value, Value)
	{
	}

	inline Vector3(const float* Array)
		: Float3(Array)
	{
	}

	inline Vector3(const Vector2& Other, float Z = 0.0f)
		: Float3(Other.x, Other.y, Z)
	{
	}

#if defined(USE_SSE)
	VECTOR_MEMBER_FUNCTIONS_TRANSFORM_DECLARE(3)
	VECTOR_MEMBER_FUNCTIONS(3)
	VECTOR_MEMBER_FUNCTIONS_CROSS(3)
#else
	inline float LengthSq()
	{
		return x * x + y * y + z * z;
	}

	inline float Length()
	{
		return std::sqrtf(LengthSq());
	}

	inline void Normalize()
	{
		float Factor = 0.0f;
		float Length = Length();
		if (Length > 0.0f)
		{
			Factor = 1.0f / Length;
		}
		x *= Factor;
		y *= Factor;
		z *= Factor;
	}

	inline void Cross(const Vector3 &Right)
	{
		float X = y * Right.z - z * Right.y;
		float Y = z * Right.x - x * Right.z;
		float Z = x * Right.y - y * Right.x;

		x = X;
		y = Y;
		z = Z;
	}

	inline float Dot(const Vector3 &Right) const
	{
		return x * Right.x + y * Right.y + z * Right.z;
	}

	inline void Negate()
	{
		x = -x;
		y = -y;
		z = -z;
	}

	inline void operator+=(const Vector3 &Right)
	{
		x += Right.x;
		y += Right.y;
		z += Right.z;
	}

	inline void operator-=(const Vector3 &Right)
	{
		x -= Right.x;
		y -= Right.y;
		z -= Right.z;
	}

	inline void operator*=(float Right)
	{
		x *= Right;
		y *= Right;
		z *= Right;
	}

	inline void operator*=(const Vector3 &Right)
	{
		x *= Right.x;
		y *= Right.y;
		z *= Right.z;
	}
#endif
	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_NVP(x),
			CEREAL_NVP(y),
			CEREAL_NVP(z)
		);
	}
};

#if defined(USE_SSE)
	VECTOR_GLOBAL_FUNCTIONS(3)
	VECTOR_GLOBAL_FUNCTIONS_CROSS(3)
#else
inline Vector3 operator+(const Vector3 &Left, const Vector3 &Right)
{
	return Vector3(Left.x + Right.x, Left.y + Right.y, Left.z + Right.z);
}

inline Vector3 operator-(const Vector3 &Left, const Vector3 &Right)
{
	return Vector3(Left.x - Right.x, Left.y - Right.y, Left.z - Right.z);
}

inline Vector3 operator*(const Vector3 &Left, float Right)
{
	return Vector3(Left.x * Right, Left.y * Right, Left.z * Right);
}

inline Vector3 operator*(const Vector3 &Left, const Vector3 &Right)
{
	return Vector3(Left.x * Right.x, Left.y * Right.y, Left.z * Right.z);
}

inline bool operator==(const Vector3 &Left, const Vector3 &Right)
{
	return (
		((Left.x - Right.x) <= Math::Epsilon) &&
		((Left.y - Right.y) <= Math::Epsilon) &&
		((Left.z - Right.z) <= Math::Epsilon)
		);
}

inline bool operator>=(const Vector3 &Left, const Vector3 &Right)
{
	return (
		((Left.x - Right.x) >= Math::Epsilon) &&
		((Left.y - Right.y) >= Math::Epsilon) &&
		((Left.z - Right.z) >= Math::Epsilon)
		);
}

inline bool operator>(const Vector3 &Left, const Vector3 &Right)
{
	return (
		((Left.x - Right.x) > Math::Epsilon) &&
		((Left.y - Right.y) > Math::Epsilon) &&
		((Left.z - Right.z) > Math::Epsilon)
		);
}

inline Vector3 Min(const Vector3 &Left, const Vector3 &Right)
{
	return Left >= Right ? Right : Left;
}

inline Vector3 Max(const Vector3 &Left, const Vector3 &Right)
{
	return Left >= Right ? Left : Right;
}

inline Vector3 Normalize(const Vector3 &Vec)
{
	Vector3 Ret(Vec);
	Ret.Normalize();

	return Ret;
}

inline float Dot(const Vector3 &Left, const Vector3 &Right)
{
	return Left.x * Right.x + Left.y * Right.y + Left.z * Right.z;
}

inline Vector3 Cross(const Vector3 &Left, const Vector3 &Right)
{
	float X = Left.y * Right.z - Left.z * Right.y;
	float Y = Left.z * Right.x - Left.x * Right.z;
	float Z = Left.x * Right.y - Left.y * Right.x;

	return Vector3(X, Y, Z);
}

inline Vector3 Negate(const Vector3 &Vec)
{
	Vector3 Ret(Vec);
	Ret.negate();

	return Ret;
}
#endif

NAMESPACE_END(Math)
