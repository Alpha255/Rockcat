#pragma once

#include "Core/Math/Vector3.h"

NAMESPACE_START(Gear)
NAMESPACE_START(Math)

class Vector4 : public Float4
{
public:
	inline Vector4()
		: Float4(0.0f, 0.0f, 0.0f, 0.0f)
	{
	}

	inline Vector4(float32_t X, float32_t Y, float32_t Z, float32_t W)
		: Float4(X, Y, Z, W)
	{
	}

	inline Vector4(float32_t Value)
		: Float4(Value, Value, Value, Value)
	{
	}

	inline Vector4(const float32_t* Array)
		: Float4(Array)
	{
	}

	inline Vector4(const Vector2& XY, float32_t Z = 0.0f, float32_t W = 0.0f)
		: Float4(XY.x, XY.y, Z, W)
	{
	}

	inline Vector4(const Vector3& XYZ, float32_t W = 0.0f)
		: Float4(XYZ.x, XYZ.y, XYZ.z, W)
	{
	}

#if defined(USE_SSE)
	void Transform(const class Matrix& Trans);

	inline Vector4 Cross(const Vector4& Other)
	{
		Vector4 Result;
		VECTOR_STORE(4, &Result, DirectX::XMVector3Cross(VECTOR_LOAD(4, this), VECTOR_LOAD(4, &Other)));
		return Result;
	}

	inline Vector4 Cross(const Vector4& V1, const Vector4& V2)
	{
		Vector4 Result;
		VECTOR_STORE(4, &Result, DirectX::XMVector4Cross(VECTOR_LOAD(4, this), VECTOR_LOAD(4, &V1), VECTOR_LOAD(4, &V2)));
		return Result;
	}

	VECTOR_MEMBER_FUNCTIONS(4)
#else
	inline float32_t LengthSq()
	{
		return x * x + y * y + z * z + w * w;
	}

	inline float32_t Length()
	{
		return ::sqrtf(lengthSq());
	}

	inline void Normalize()
	{
		float32_t Factor = 0.0f;
		float32_t Length = length();
		if (Length > 0.0f)
		{
			Factor = 1.0f / Length;
		}
		x *= Factor;
		y *= Factor;
		z *= Factor;
		w *= Factor;
	}

	inline void Cross(const Vector4 &Right)
	{
		Vec3 TempLeft(x, y, z);
		Vec3 TempRight(Right.x, Right.y, Right.z);
		TempLeft.Cross(TempRight);

		x = TempLeft.x;
		y = TempLeft.y;
		z = TempLeft.z;
		w = 0.0f;
	}

	inline float32_t Dot(const Vector4 &Right) const
	{
		return x * Right.x + y * Right.y + z * Right.z + w * Right.w;
	}

	inline void Negate()
	{
		x = -x;
		y = -y;
		z = -z;
		w = -w;
	}

	inline void operator+=(const Vector4 &Right)
	{
		x += Right.x;
		y += Right.y;
		z += Right.z;
		w += Right.w;
	}

	inline void operator-=(const Vector4 &Right)
	{
		x -= Right.x;
		y -= Right.y;
		z -= Right.z;
		w -= Right.w;
	}

	inline void operator*=(float32_t Right)
	{
		x *= Right;
		y *= Right;
		z *= Right;
		w *= Right;
	}

	inline void operator*=(const Vector4 &Right)
	{
		x *= Right.x;
		y *= Right.y;
		z *= Right.z;
		w *= Right.w;
	}
#endif
	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_NVP(x),
			CEREAL_NVP(y),
			CEREAL_NVP(z),
			CEREAL_NVP(w)
		);
	}
};

#if defined(USE_SSE)
	VECTOR_GLOBAL_FUNCTIONS(4)

	inline Vector4 Cross(const Vector4& V0, const Vector4& V1)
	{
		Vector4 Result;
		VECTOR_STORE(4, &Result, DirectX::XMVector3Cross(VECTOR_LOAD(4, &V0), VECTOR_LOAD(4, &V1)));
		return Result;
	}

	inline Vector4 Cross(const Vector4& V0, const Vector4& V1, const Vector4& V2)
	{        
		Vector4 Result;
		VECTOR_STORE(4, &Result, DirectX::XMVector4Cross(VECTOR_LOAD(4, &V0), VECTOR_LOAD(4, &V1), VECTOR_LOAD(4, &V2)));
		return Result;
	}
#else
inline Vector4 operator+(const Vector4 &Left, const Vector4 &Right)
{
	return Vector4(Left.x + Right.x, Left.y + Right.y, Left.z + Right.z, Left.w + Right.w);
}

inline Vector4 operator-(const Vector4 &Left, const Vector4 &Right)
{
	return Vector4(Left.x - Right.x, Left.y - Right.y, Left.z - Right.z, Left.w - Right.w);
}

inline Vector4 operator*(const Vector4 &Left, float32_t Right)
{
	return Vector4(Left.x * Right, Left.y * Right, Left.z * Right, Left.w * Right);
}

inline Vector4 operator*(const Vector4 &Left, const Vector4 &Right)
{
	return Vector4(Left.x * Right.x, Left.y * Right.y, Left.z * Right.z, Left.w * Right.w);
}

inline bool operator==(const Vector4 &Left, const Vector4 &Right)
{
	return (
		((Left.x - Right.x) <= Math::Epsilon) &&
		((Left.y - Right.y) <= Math::Epsilon) &&
		((Left.z - Right.z) <= Math::Epsilon) &&
		((Left.w - Right.w) <= Math::Epsilon)
		);
}

inline bool operator>=(const Vector4 &Left, const Vector4 &Right)
{
	return (
		((Left.x - Right.x) >= Math::Epsilon) &&
		((Left.y - Right.y) >= Math::Epsilon) &&
		((Left.z - Right.z) >= Math::Epsilon) &&
		((Left.w - Right.w) >= Math::Epsilon)
		);
}

inline bool operator>(const Vector4 &Left, const Vector4 &Right)
{
	return (
		((Left.x - Right.x) > Math::Epsilon) &&
		((Left.y - Right.y) > Math::Epsilon) &&
		((Left.z - Right.z) > Math::Epsilon) &&
		((Left.w - Right.w) > Math::Epsilon)
		);
}

inline Vector4 Min(const Vector4 &Left, const Vector4 &Right)
{
	return Left >= Right ? Right : Left;
}

inline Vector4 Max(const Vector4 &Left, const Vector4 &Right)
{
	return Left >= Right ? Left : Right;
}

inline Vector4 Normalize(const Vector4 &Vec)
{
	Vector4 Ret(Vec);
	Ret.Normalize();

	return Ret;
}

inline float32_t Dot(const Vector4 &Left, const Vector4 &Right)
{
	return Left.x * Right.x + Left.y * Right.y + Left.z * Right.z + Left.w * Right.w;
}

inline Vector4 Cross(const Vector4 &Left, const Vector4 &Right)
{
	Vec3 TempLeft(Left.x, Left.y, Left.z);
	Vec3 TempRight(Right.x, Right.y, Right.z);
	TempLeft.Cross(TempRight);

	return Vector4(TempLeft);
}

inline Vector4 Negate(const Vector4 &Vec)
{
	Vector4 Ret(Vec);
	Ret.Negate();

	return Ret;
}
#endif

NAMESPACE_END(Math)
NAMESPACE_END(Gear)
