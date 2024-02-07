#pragma once

#include "Core/Definitions.h"
#include "Core/Cereal.h"

#define USE_SSE 1
//#define FORCE_ALIGN_16

#if !USE_SSE
	#define _XM_NO_INTRINSICS_
#endif

#include <DirectXMath.h>
#include <DirectXCollision.h>

NAMESPACE_START(Math)

/***************************************************************************************
	XMLoadFloat4A _mm_load_ps : Loads four f32 values from aligned memory into a __m128. 
								If the pointer is not aligned to a 128-bit boundary 
								(16 bytes) a general protection fault will be triggered 
								(fatal program crash). Use _mm_loadu_ps for potentially 
								unaligned memory.
	XMLoadFloat4  _mm_loadu_ps: Loads four f32 values from memory into a __m128. 
								There are no restrictions on memory alignment. 
								For aligned memory _mm_load_ps may be faster.
****************************************************************************************/

#if defined(FORCE_ALIGN_16)
	#define VECTOR_LOAD(Dimension, Vec) DirectX::XMLoadFloat##Dimension##A(Vec)
	#define VECTOR_STORE(Dimension, Dst, Src) DirectX::XMStoreFloat##Dimension##A(Dst, Src)
	#define MATRIX_LOAD(Matrix) DirectX::XMLoadFloat4x4A(Matrix)
	#define MATRIX_STORE(Dst, Src) DirectX::XMStoreFloat4x4A(Dst, Src)
	
	using Float2 = DirectX::XMFLOAT2A;
	using Float3 = DirectX::XMFLOAT3A;
	using Float4 = DirectX::XMFLOAT4A;
	using Float4x4 = DirectX::XMFLOAT4X4A;
#else
	#define VECTOR_LOAD(Dimension, Vec) DirectX::XMLoadFloat##Dimension(Vec)
	#define VECTOR_STORE(Dimension, Dst, Src) DirectX::XMStoreFloat##Dimension(Dst, Src)
	#define MATRIX_LOAD(Matrix) DirectX::XMLoadFloat4x4(Matrix)
	#define MATRIX_STORE(Dst, Src) DirectX::XMStoreFloat4x4(Dst, Src)

	using Float2 = DirectX::XMFLOAT2;
	using Float3 = DirectX::XMFLOAT3;
	using Float4 = DirectX::XMFLOAT4;
	using Float4x4 = DirectX::XMFLOAT4X4;
#endif

	using Int2 = DirectX::XMINT2;
	using Int3 = DirectX::XMINT3;
	using Int4 = DirectX::XMINT4;
	using UInt2 = DirectX::XMUINT2;
	using UInt3 = DirectX::XMUINT3;
	using UInt4 = DirectX::XMUINT4;

#define VECTOR_EQUAL(Dimension, Left, Right)               DirectX::XMVector##Dimension##Equal(Left, Right)
#define VECTOR_NOT_EQUAL(Dimension, Left, Right)           DirectX::XMVector##Dimension##NotEqual(Left, Right)
#define VECTOR_GREATER(Dimension, Left, Right)             DirectX::XMVector##Dimension##Greater(Left, Right)
#define VECTOR_GREATER_OR_EQUAL(Dimension, Left, Right)    DirectX::XMVector##Dimension##GreaterOrEqual(Left, Right)
#define VECTOR_LESS(Dimension, Left, Right)                DirectX::XMVector##Dimension##Less(Left, Right)
#define VECTOR_LESS_OR_EQUAL(Dimension, Left, Right)       DirectX::XMVector##Dimension##LessOrEqual(Left, Right)
#define VECTOR_CROSS(Dimension, Left, Right)               DirectX::XMVector##Dimension##Cross(Left, Right)
#define VECTOR_DOT(Dimension, Left, Right)                 DirectX::XMVector##Dimension##Dot(Left, Right)
#define VECTOR_IS_NAN(Dimension, Vec)                      DirectX::XMVector##Dimension##IsNaN(Vec)
#define VECTOR_IS_INFINITE(Dimension, Vec)                 DirectX::XMVector##Dimension##IsInfinite(Vec)
#define VECTOR_LENGTH(Dimension, Vec)                      DirectX::XMVector##Dimension##Length(Vec)
#define VECTOR_LENGTH_EST(Dimension, Vec)                  DirectX::XMVector##Dimension##LengthEst(Vec)  /// Est functions offer increased performance at the expense of reduced accuracy
#define VECTOR_RECIPROCAL_LENGTH(Dimension, Vec)           DirectX::XMVector##Dimension##ReciprocalLength(Vec)
#define VECTOR_RECIPROCAL_LENGTH_EST(Dimension, Vec)       DirectX::XMVector##Dimension##ReciprocalLengthEst(Vec)
#define VECTOR_LENGTH_SQ(Dimension, Vec)                   DirectX::XMVector##Dimension##LengthSq(Vec)
#define VECTOR_NORMALIZE(Dimension, Vec)                   DirectX::XMVector##Dimension##Normalize(Vec)
#define VECTOR_CLAMP_LENGTH(Dimension, Vec, Min, Max)      DirectX::XMVector##Dimension##ClampLength(Vec, Min, Max)
#define VECTOR_REFLECT(Dimension, Vec, Normal)             DirectX::XMVector##Dimension##Reflect(Vec, Normal)
#define VECTOR_REFRACT(Dimension, Vec, Normal, Index)      DirectX::XMVector##Dimension##Refract(Vec, Normal, Index)
#define VECTOR_ORTHOGONAL(Dimension, Vec)                  DirectX::XMVector##Dimension##Orthogonal(Vec)
#define VECTOR_TRANSFORM(Dimension, Vec, Trans)            DirectX::XMVector##Dimension##Transform(Vec, Trans)
#define VECTOR_TRANSFORM_COORD(Dimension, Vec, Trans)      DirectX::XMVector##Dimension##TransformCoord(Vec, Trans)
#define VECTOR_TRANSFORM_NORMAL(Dimension, Vec, Trans)     DirectX::XMVector##Dimension##TransformNormal(Vec, Trans)

#define VECTOR_MEMBER_FUNCTIONS(Dimension)                                                                                                       \
inline void operator+=(const Vector##Dimension& Right)                                                                                           \
{                                                                                                                                                \
	VECTOR_STORE(Dimension, this, DirectX::XMVectorAdd(VECTOR_LOAD(Dimension, this), VECTOR_LOAD(Dimension, &Right)));                           \
}                                                                                                                                                \
inline void operator-=(const Vector##Dimension& Right)                                                                                           \
{                                                                                                                                                \
	VECTOR_STORE(Dimension, this, DirectX::XMVectorSubtract(VECTOR_LOAD(Dimension, this), VECTOR_LOAD(Dimension, &Right)));                      \
}                                                                                                                                                \
inline void operator*=(const float Right)                                                                                                    \
{                                                                                                                                                \
	VECTOR_STORE(Dimension, this, DirectX::XMVectorScale(VECTOR_LOAD(Dimension, this), Right));                                                  \
}                                                                                                                                                \
inline void operator*=(const Vector##Dimension& Right)                                                                                           \
{                                                                                                                                                \
	VECTOR_STORE(Dimension, this, DirectX::XMVectorMultiply(VECTOR_LOAD(Dimension, this), VECTOR_LOAD(Dimension, &Right)));                      \
}                                                                                                                                                \
inline void operator/=(const float Right)                                                                                                    \
{                                                                                                                                                \
	assert(std::fpclassify(Right) != FP_ZERO);                                                                                                   \
	VECTOR_STORE(Dimension, this, DirectX::XMVectorScale(VECTOR_LOAD(Dimension, this), 1.0f / Right));                                           \
}                                                                                                                                                \
inline void operator/=(const Vector##Dimension& Right)                                                                                           \
{                                                                                                                                                \
	VECTOR_STORE(Dimension, this, DirectX::XMVectorDivide(VECTOR_LOAD(Dimension, this), VECTOR_LOAD(Dimension, &Right)));                        \
}                                                                                                                                                \
inline float Dot(const Vector##Dimension& Right) const                                                                                       \
{                                                                                                                                                \
	return DirectX::XMVectorGetX(VECTOR_DOT(Dimension, VECTOR_LOAD(Dimension, this), VECTOR_LOAD(Dimension, &Right)));                           \
}                                                                                                                                                \
inline void Normalize()                                                                                                                          \
{                                                                                                                                                \
	VECTOR_STORE(Dimension, this, VECTOR_NORMALIZE(Dimension, VECTOR_LOAD(Dimension, this)));                                                    \
}                                                                                                                                                \
inline void Negate()                                                                                                                             \
{                                                                                                                                                \
	VECTOR_STORE(Dimension, this, DirectX::XMVectorNegate(VECTOR_LOAD(Dimension, this)));                                                        \
}                                                                                                                                                \
inline float Length() const                                                                                                                  \
{                                                                                                                                                \
	return DirectX::XMVectorGetX(VECTOR_LENGTH(Dimension, VECTOR_LOAD(Dimension, this)));                                                        \
}                                                                                                                                                \
inline float LengthSq() const                                                                                                                \
{                                                                                                                                                \
	return DirectX::XMVectorGetX(VECTOR_LENGTH_SQ(Dimension, VECTOR_LOAD(Dimension, this)));                                                     \
}                                                                                                                                                \
inline float LengthEst() const                                                                                                               \
{                                                                                                                                                \
	return DirectX::XMVectorGetX(VECTOR_LENGTH_EST(Dimension, VECTOR_LOAD(Dimension, this)));                                                    \
}                                                                                                                                                \
inline float ReciprocalLength() const                                                                                                        \
{                                                                                                                                                \
	return DirectX::XMVectorGetX(VECTOR_RECIPROCAL_LENGTH(Dimension, VECTOR_LOAD(Dimension, this)));                                             \
}                                                                                                                                                \
inline float ReciprocalLengthEst() const                                                                                                     \
{                                                                                                                                                \
	return DirectX::XMVectorGetX(VECTOR_RECIPROCAL_LENGTH_EST(Dimension, VECTOR_LOAD(Dimension, this)));                                         \
}                                                                                                                                                \
inline bool IsNaN() const                                                                                                                     \
{                                                                                                                                                \
	return VECTOR_IS_NAN(Dimension, VECTOR_LOAD(Dimension, this));                                                                               \
}                                                                                                                                                \
inline bool IsInfinite() const                                                                                                                \
{                                                                                                                                                \
	return VECTOR_IS_INFINITE(Dimension, VECTOR_LOAD(Dimension, this));                                                                          \
}                                                                                                                                                \
inline void ClampLength(float Min, float Max)                                                                                            \
{                                                                                                                                                \
	VECTOR_STORE(Dimension, this, VECTOR_CLAMP_LENGTH(Dimension, VECTOR_LOAD(Dimension, this), Min, Max));                                       \
}                                                                                                                                                \
inline Vector##Dimension Reflect(const Vector##Dimension& Normal) const                                                                          \
{                                                                                                                                                \
	Vector##Dimension Result;                                                                                                                    \
	VECTOR_STORE(Dimension, &Result, VECTOR_REFLECT(Dimension, VECTOR_LOAD(Dimension, this), VECTOR_LOAD(Dimension, &Normal)));                  \
	return Result;                                                                                                                               \
}                                                                                                                                                \
inline Vector##Dimension Refract(const Vector##Dimension& Normal, float RefractionIndex) const                                               \
{                                                                                                                                                \
	Vector##Dimension Result;                                                                                                                    \
	VECTOR_STORE(Dimension, &Result, VECTOR_REFRACT(Dimension, VECTOR_LOAD(Dimension, this), VECTOR_LOAD(Dimension, &Normal), RefractionIndex)); \
	return Result;                                                                                                                               \
}                                                                                                                                                \
inline Vector##Dimension Orthogonal() const                                                                                                      \
{                                                                                                                                                \
	Vector##Dimension Result;                                                                                                                    \
	VECTOR_STORE(Dimension, &Result, VECTOR_ORTHOGONAL(Dimension, VECTOR_LOAD(Dimension, this)));                                                \
	return Result;                                                                                                                               \
}                                                                                                                                                \
inline Vector##Dimension Round() const                                                                                                           \
{                                                                                                                                                \
	Vector##Dimension Result;                                                                                                                    \
	VECTOR_STORE(Dimension, &Result, DirectX::XMVectorRound(VECTOR_LOAD(Dimension, this)));                                                      \
	return Result;                                                                                                                               \
}                                                                                                                                                \
inline Vector##Dimension Truncate() const                                                                                                        \
{                                                                                                                                                \
	Vector##Dimension Result;                                                                                                                    \
	VECTOR_STORE(Dimension, &Result, DirectX::XMVectorTruncate(VECTOR_LOAD(Dimension, this)));                                                   \
	return Result;                                                                                                                               \
}                                                                                                                                                \
inline Vector##Dimension Floor() const                                                                                                           \
{                                                                                                                                                \
	Vector##Dimension Result;                                                                                                                    \
	VECTOR_STORE(Dimension, &Result, DirectX::XMVectorFloor(VECTOR_LOAD(Dimension, this)));                                                      \
	return Result;                                                                                                                               \
}                                                                                                                                                \
inline Vector##Dimension Ceiling() const                                                                                                         \
{                                                                                                                                                \
	Vector##Dimension Result;                                                                                                                    \
	VECTOR_STORE(Dimension, &Result, DirectX::XMVectorCeiling(VECTOR_LOAD(Dimension, this)));                                                    \
	return Result;                                                                                                                               \
}                                                                                                                                                \
inline Vector##Dimension Saturate() const                                                                                                        \
{                                                                                                                                                \
	Vector##Dimension Result;                                                                                                                    \
	VECTOR_STORE(Dimension, &Result, DirectX::XMVectorSaturate(VECTOR_LOAD(Dimension, this)));                                                   \
	return Result;                                                                                                                               \
}                                                                                                                                                \
inline Vector##Dimension Clamp(const Vector##Dimension& Min, const Vector##Dimension& Max) const                                                 \
{                                                                                                                                                \
	Vector##Dimension Result;                                                                                                                    \
	DirectX::XMVECTOR MinV = VECTOR_LOAD(Dimension, &Min);                                                                                       \
	DirectX::XMVECTOR MaxV = VECTOR_LOAD(Dimension, &Max);                                                                                       \
	VECTOR_STORE(Dimension, &Result, DirectX::XMVectorClamp(VECTOR_LOAD(Dimension, this), MinV, MaxV));                                          \
	return Result;                                                                                                                               \
}

#define VECTOR_MEMBER_FUNCTIONS_CROSS(Dimension)                                                                                                 \
inline Vector##Dimension Cross(const Vector##Dimension& Right) const                                                                             \
{                                                                                                                                                \
	Vector##Dimension Result;                                                                                                                    \
	VECTOR_STORE(Dimension, &Result, VECTOR_CROSS(Dimension, VECTOR_LOAD(Dimension, this), VECTOR_LOAD(Dimension, &Right)));                     \
	return Result;                                                                                                                               \
}                                                                                                                                                

#define VECTOR_MEMBER_FUNCTIONS_TRANSFORM_DECLARE(Dimension) \
void Transform(const class Matrix& Trans);                   \
void TransformNormal(const class Matrix& Trans);             \
void TransformCoord(const class Matrix& Trans);              \

#define VECTOR_MEMBER_FUNCTIONS_TRANSFORM(Dimension)                                                                      \
void Vector##Dimension::Transform(const Matrix& Trans)                                                                    \
{                                                                                                                         \
	VECTOR_STORE(Dimension, this, VECTOR_TRANSFORM(Dimension, VECTOR_LOAD(Dimension, this), MATRIX_LOAD(&Trans)));        \
}                                                                                                                         \
void Vector##Dimension::TransformNormal(const Matrix& Trans)                                                              \
{                                                                                                                         \
	VECTOR_STORE(Dimension, this, VECTOR_TRANSFORM_NORMAL(Dimension, VECTOR_LOAD(Dimension, this), MATRIX_LOAD(&Trans))); \
}                                                                                                                         \
void Vector##Dimension::TransformCoord(const Matrix& Trans)                                                               \
{                                                                                                                         \
	VECTOR_STORE(Dimension, this, VECTOR_TRANSFORM_COORD(Dimension, VECTOR_LOAD(Dimension, this), MATRIX_LOAD(&Trans)));  \
}

#define VECTOR_GLOBAL_FUNCTIONS(Dimension)                                                                                                                    \
inline Vector##Dimension operator+(const Vector##Dimension& Left, const Vector##Dimension& Right)                                                             \
{                                                                                                                                                             \
	Vector##Dimension Result;                                                                                                                                 \
	VECTOR_STORE(Dimension, &Result, DirectX::XMVectorAdd(VECTOR_LOAD(Dimension, &Left), VECTOR_LOAD(Dimension, &Right)));                                    \
	return Result;                                                                                                                                            \
}                                                                                                                                                             \
inline Vector##Dimension operator-(const Vector##Dimension &Left, const Vector##Dimension &Right)                                                             \
{                                                                                                                                                             \
	Vector##Dimension Result;                                                                                                                                 \
	VECTOR_STORE(Dimension, &Result, DirectX::XMVectorSubtract(VECTOR_LOAD(Dimension, &Left), VECTOR_LOAD(Dimension, &Right)));                               \
	return Result;                                                                                                                                            \
}                                                                                                                                                             \
inline Vector##Dimension operator*(const Vector##Dimension& Left, const float Right)                                                                      \
{                                                                                                                                                             \
	Vector##Dimension Result;                                                                                                                                 \
	VECTOR_STORE(Dimension, &Result, DirectX::XMVectorScale(VECTOR_LOAD(Dimension, &Left), Right));                                                           \
	return Result;                                                                                                                                            \
}                                                                                                                                                             \
inline Vector##Dimension operator*(const Vector##Dimension& Left, const Vector##Dimension& Right)                                                             \
{                                                                                                                                                             \
	Vector##Dimension Result;                                                                                                                                 \
	VECTOR_STORE(Dimension, &Result, DirectX::XMVectorMultiply(VECTOR_LOAD(Dimension, &Left), VECTOR_LOAD(Dimension, &Right)));                               \
	return Result;                                                                                                                                            \
}                                                                                                                                                             \
inline Vector##Dimension operator/(const Vector##Dimension& Left, const float Right)                                                                      \
{                                                                                                                                                             \
	assert(std::fpclassify(Right) != FP_ZERO);                                                                                                                \
	Vector##Dimension Result;                                                                                                                                 \
	VECTOR_STORE(Dimension, &Result, DirectX::XMVectorScale(VECTOR_LOAD(Dimension, &Left), 1.0f / Right));                                                    \
	return Result;                                                                                                                                            \
}                                                                                                                                                             \
inline Vector##Dimension operator/(const Vector##Dimension& Left, const Vector##Dimension& Right)                                                             \
{                                                                                                                                                             \
	Vector##Dimension Result;                                                                                                                                 \
	VECTOR_STORE(Dimension, &Result, DirectX::XMVectorDivide(VECTOR_LOAD(Dimension, &Left), VECTOR_LOAD(Dimension, &Right)));                                 \
	return Result;                                                                                                                                            \
}                                                                                                                                                             \
inline bool operator==(const Vector##Dimension& Left, const Vector##Dimension& Right)                                                                      \
{                                                                                                                                                             \
	return VECTOR_EQUAL(Dimension, VECTOR_LOAD(Dimension, &Left), VECTOR_LOAD(Dimension, &Right));                                                            \
}                                                                                                                                                             \
inline bool operator!=(const Vector##Dimension& Left, const Vector##Dimension& Right)                                                                      \
{                                                                                                                                                             \
	return VECTOR_NOT_EQUAL(Dimension, VECTOR_LOAD(Dimension, &Left), VECTOR_LOAD(Dimension, &Right));                                                        \
}                                                                                                                                                             \
inline bool operator>(const Vector##Dimension& Left, const Vector##Dimension& Right)                                                                       \
{                                                                                                                                                             \
	return VECTOR_GREATER(Dimension, VECTOR_LOAD(Dimension, &Left), VECTOR_LOAD(Dimension, &Right));                                                          \
}                                                                                                                                                             \
inline bool operator>=(const Vector##Dimension& Left, const Vector##Dimension& Right)                                                                      \
{                                                                                                                                                             \
	return VECTOR_GREATER_OR_EQUAL(Dimension, VECTOR_LOAD(Dimension, &Left), VECTOR_LOAD(Dimension, &Right));                                                 \
}                                                                                                                                                             \
inline bool operator<(const Vector##Dimension& Left, const Vector##Dimension& Right)                                                                       \
{                                                                                                                                                             \
	return VECTOR_LESS(Dimension, VECTOR_LOAD(Dimension, &Left), VECTOR_LOAD(Dimension, &Right));                                                             \
}                                                                                                                                                             \
inline bool operator<=(const Vector##Dimension& Left, const Vector##Dimension& Right)                                                                      \
{                                                                                                                                                             \
	return VECTOR_LESS_OR_EQUAL(Dimension, VECTOR_LOAD(Dimension, &Left), VECTOR_LOAD(Dimension, &Right));                                                    \
}                                                                                                                                                             \
inline float Dot(const Vector##Dimension& Left, const Vector##Dimension& Right)                                                                           \
{                                                                                                                                                             \
	return DirectX::XMVectorGetX(VECTOR_DOT(Dimension, VECTOR_LOAD(Dimension, &Left), VECTOR_LOAD(Dimension, &Right)));                                       \
}                                                                                                                                                             \
inline Vector##Dimension Normalize(const Vector##Dimension& Vec)                                                                                              \
{                                                                                                                                                             \
	Vector##Dimension Result;                                                                                                                                 \
	VECTOR_STORE(Dimension, &Result, VECTOR_NORMALIZE(Dimension, VECTOR_LOAD(Dimension, &Vec)));                                                              \
	return Result;                                                                                                                                            \
}                                                                                                                                                             \
inline Vector##Dimension Negate(Vector##Dimension& Vec)                                                                                                       \
{                                                                                                                                                             \
	Vector##Dimension Result;                                                                                                                                 \
	VECTOR_STORE(Dimension, &Result, DirectX::XMVectorNegate(VECTOR_LOAD(Dimension, &Vec)));                                                                  \
	return Result;                                                                                                                                            \
}                                                                                                                                                             \
inline float Length(const Vector##Dimension& Vec)                                                                                                         \
{                                                                                                                                                             \
	return DirectX::XMVectorGetX(VECTOR_LENGTH(Dimension, VECTOR_LOAD(Dimension, &Vec)));                                                                     \
}                                                                                                                                                             \
inline float LengthSq(const Vector##Dimension& Vec)                                                                                                       \
{                                                                                                                                                             \
	return DirectX::XMVectorGetX(VECTOR_LENGTH_SQ(Dimension, VECTOR_LOAD(Dimension, &Vec)));                                                                  \
}                                                                                                                                                             \
inline float LengthEst(const Vector##Dimension& Vec)                                                                                                      \
{                                                                                                                                                             \
	return DirectX::XMVectorGetX(VECTOR_LENGTH_EST(Dimension, VECTOR_LOAD(Dimension, &Vec)));                                                                 \
}                                                                                                                                                             \
inline float ReciprocalLength(const Vector##Dimension& Vec)                                                                                               \
{                                                                                                                                                             \
	return DirectX::XMVectorGetX(VECTOR_RECIPROCAL_LENGTH(Dimension, VECTOR_LOAD(Dimension, &Vec)));                                                          \
}                                                                                                                                                             \
inline float ReciprocalLengthEst(const Vector##Dimension& Vec)                                                                                            \
{                                                                                                                                                             \
	return DirectX::XMVectorGetX(VECTOR_RECIPROCAL_LENGTH_EST(Dimension, VECTOR_LOAD(Dimension, &Vec)));                                                      \
}                                                                                                                                                             \
inline bool IsNaN(const Vector##Dimension& Vec)                                                                                                            \
{                                                                                                                                                             \
	return VECTOR_IS_NAN(Dimension, VECTOR_LOAD(Dimension, &Vec));                                                                                            \
}                                                                                                                                                             \
inline bool IsInfinite(const Vector##Dimension& Vec)                                                                                                       \
{                                                                                                                                                             \
	return VECTOR_IS_INFINITE(Dimension, VECTOR_LOAD(Dimension, &Vec));                                                                                       \
}                                                                                                                                                             \
inline Vector##Dimension ClampLength(const Vector##Dimension& Vec, float Min, float Max)                                                              \
{                                                                                                                                                             \
	Vector##Dimension Result;                                                                                                                                 \
	VECTOR_STORE(Dimension, &Result, VECTOR_CLAMP_LENGTH(Dimension, VECTOR_LOAD(Dimension, &Vec), Min, Max));                                                 \
	return Result;                                                                                                                                            \
}                                                                                                                                                             \
inline Vector##Dimension Reflect(const Vector##Dimension& Vec, const Vector##Dimension& Normal)                                                               \
{                                                                                                                                                             \
	Vector##Dimension Result;                                                                                                                                 \
	VECTOR_STORE(Dimension, &Result, VECTOR_REFLECT(Dimension, VECTOR_LOAD(Dimension, &Vec), VECTOR_LOAD(Dimension, &Normal)));                               \
	return Result;                                                                                                                                            \
}                                                                                                                                                             \
inline Vector##Dimension Refract(const Vector##Dimension& Vec, const Vector##Dimension& Normal, float RefractionIndex)                                    \
{                                                                                                                                                             \
	Vector##Dimension Result;                                                                                                                                 \
	VECTOR_STORE(Dimension, &Result, VECTOR_REFRACT(Dimension, VECTOR_LOAD(Dimension, &Vec), VECTOR_LOAD(Dimension, &Normal), RefractionIndex));              \
	return Result;                                                                                                                                            \
}                                                                                                                                                             \
inline Vector##Dimension Orthogonal(const Vector##Dimension& Vec)                                                                                             \
{                                                                                                                                                             \
	Vector##Dimension Result;                                                                                                                                 \
	VECTOR_STORE(Dimension, &Result, VECTOR_ORTHOGONAL(Dimension, VECTOR_LOAD(Dimension, &Vec)));                                                             \
	return Result;                                                                                                                                            \
}                                                                                                                                                             \
inline Vector##Dimension Min(const Vector##Dimension& Left, const Vector##Dimension& Right)                                                                   \
{                                                                                                                                                             \
	Vector##Dimension Result;                                                                                                                                 \
	VECTOR_STORE(Dimension, &Result, DirectX::XMVectorMin(VECTOR_LOAD(Dimension, &Left), VECTOR_LOAD(Dimension, &Right)));                                    \
	return Result;                                                                                                                                            \
}                                                                                                                                                             \
inline Vector##Dimension Max(const Vector##Dimension& Left, const Vector##Dimension& Right)                                                                   \
{                                                                                                                                                             \
	Vector##Dimension Result;                                                                                                                                 \
	VECTOR_STORE(Dimension, &Result, DirectX::XMVectorMax(VECTOR_LOAD(Dimension, &Left), VECTOR_LOAD(Dimension, &Right)));                                    \
	return Result;                                                                                                                                            \
}                                                                                                                                                             \
inline Vector##Dimension Lerp(const Vector##Dimension &Left, const Vector##Dimension& Right, float factor)                                                \
{                                                                                                                                                             \
	Vector##Dimension Result;                                                                                                                                 \
	VECTOR_STORE(Dimension, &Result, DirectX::XMVectorLerp(VECTOR_LOAD(Dimension, &Left), VECTOR_LOAD(Dimension, &Right), factor));                           \
	return Result;                                                                                                                                            \
}                                                                                                                                                             \
inline Vector##Dimension LerpV(const Vector##Dimension & Left, const Vector##Dimension & Right, const Vector##Dimension& Factor)                              \
{                                                                                                                                                             \
	Vector##Dimension Result;                                                                                                                                 \
	VECTOR_STORE(Dimension, &Result, DirectX::XMVectorLerpV(VECTOR_LOAD(Dimension, &Left), VECTOR_LOAD(Dimension, &Right), VECTOR_LOAD(Dimension, &Factor))); \
	return Result;                                                                                                                                            \
}

#define VECTOR_GLOBAL_FUNCTIONS_CROSS(Dimension)                                                                              \
inline Vector##Dimension Cross(const Vector##Dimension& Left, const Vector##Dimension& Right)                                 \
{                                                                                                                             \
	Vector##Dimension Result;                                                                                                 \
	VECTOR_STORE(Dimension, &Result, VECTOR_CROSS(Dimension, VECTOR_LOAD(Dimension, &Left), VECTOR_LOAD(Dimension, &Right))); \
	return Result;                                                                                                            \
}

NAMESPACE_END(Math)
