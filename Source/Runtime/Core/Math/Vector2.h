#pragma once

#include "Runtime/Core/Math/Vector.h"

NAMESPACE_START(Math)

class Vector2 : public Float2
{
public:
	inline Vector2()
		: Float2(0.0f, 0.0f)
	{
	}

	inline Vector2(float32_t Value)
		: Float2(Value, Value)
	{
	}

	inline Vector2(float32_t X, float32_t Y)
		: Float2(X, Y)
	{
	}

	inline Vector2(const float32_t* Array)
		: Float2(Array)
	{                               
	}

#if defined(USE_SSE)
	VECTOR_MEMBER_FUNCTIONS_TRANSFORM_DECLARE(2)
	VECTOR_MEMBER_FUNCTIONS(2)
	VECTOR_MEMBER_FUNCTIONS_CROSS(2)
#else
	inline float32_t LengthSq()
	{
		return x * x + y * y;
	}

	inline float32_t Length()
	{
		return std::sqrtf(LengthSq());
	}

	inline void Normalize()
	{
		float32_t Factor = 0.0f;
		float32_t Length = Length();
		if (Length > 0.0f)
		{
			Factor = 1.0f / Length;
		}
		x *= Factor;
		y *= Factor;
	}

	inline Vector2 Cross(const Vector2& Right)
	{
		float32_t Value = x * Right.y - y * Right.x;
		x = y = Value;
	}

	inline float32_t Dot(const Vector2& Right) const
	{
		return x * Right.x + y * Right.y;
	}

	inline void Negate()
	{
		x = -x;
		y = -y;
	}

	inline void operator+=(const Vector2& Right)
	{
		x += Right.x;
		y += Right.y;
	}

	inline void operator-=(const Vector2& Right)
	{
		x -= Right.x;
		y -= Right.y;
	}

	inline void operator*=(float32_t Right)
	{
		x *= Right;
		y *= Right;
	}

	inline void operator*=(const Vector2& Right)
	{
		x *= Right.x;
		y *= Right.y;
	}
#endif

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_NVP(x),
			CEREAL_NVP(y)
		);
	}
};

#if defined(USE_SSE)
	VECTOR_GLOBAL_FUNCTIONS(2)
	VECTOR_GLOBAL_FUNCTIONS_CROSS(2)
#else
inline Vector2 operator+(const Vector2 &Left, const Vector2 &Right)
{
	return Vector2(Left.x + Right.x, Left.y + Right.y);
}

inline Vector2 operator-(const Vector2 &Left, const Vector2 &Right)
{
	return Vector2(Left.x - Right.x, Left.y - Right.y);
}

inline Vector2 operator*(const Vector2 &Left, float32_t Right)
{
	return Vector2(Left.x * Right, Left.y * Right);
}

inline Vector2 operator*(const Vector2 &Left, const Vector2 &Right)
{
	return Vector2(Left.x * Right.x, Left.y * Right.y);
}

inline bool operator==(const Vector2 &Left, const Vector2 &Right)
{
	return (
		((Left.x - Right.x) <= Math::Epsilon) &&
		((Left.y - Right.y) <= Math::Epsilon)
		);
}

inline bool operator>=(const Vector2 &Left, const Vector2 &Right)
{
	return (
		((Left.x - Right.x) >= Math::Epsilon) &&
		((Left.y - Right.y) >= Math::Epsilon)
		);
}

inline bool operator>(const Vector2 &Left, const Vector2 &Right)
{
	return (
		((Left.x - Right.x) > Math::Epsilon) &&
		((Left.y - Right.y) > Math::Epsilon)
		);
}

inline Vector2 Min(const Vector2 &Left, const Vector2 &Right)
{
	return Left >= Right ? Right : Left;
}

inline Vector2 Max(const Vector2 &Left, const Vector2 &Right)
{
	return Left >= Right ? Left : Right;
}

inline Vector2 Normalize(const Vector2 &Vec)
{
	Vector2 Ret(Vec);
	Ret.Normalize();

	return Ret;
}

inline float32_t Dot(const Vector2 &Left, const Vector2 &Right)
{
	return Left.x * Right.x + Left.y * Right.y;
}

inline Vector2 Cross(const Vector2 &Left, const Vector2 &Right)
{
	float32_t Value = Left.x * Right.y - Left.y * Right.x;
	return Vector2(Value, Value);
}

inline Vector2 Negate(const Vector2 &targetVec)
{
	Vector2 Ret(targetVec);
	Ret.negate();

	return Ret;
}
#endif

NAMESPACE_END(Math)
