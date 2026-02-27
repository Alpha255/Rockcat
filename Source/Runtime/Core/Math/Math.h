#pragma once

#include "Core/Math/AABB.h"
#include "Core/Math/Color.h"
#include "Core/Math/Euler.h"
#include "Core/Math/Frustum.h"
#include "Core/Math/Matrix.h"
#include "Core/Math/OBB.h"
#include "Core/Math/Plane.h"
#include "Core/Math/Quaternion.h"
#include "Core/Math/Rect.h"
#include "Core/Math/Sphere.h"
#include "Core/Math/Transform.h"

NAMESPACE_START(Math)

static constexpr float PI = DirectX::XM_PI;
static constexpr float PI_2 = DirectX::XM_2PI;
static constexpr float PI_Inv = DirectX::XM_1DIVPI;
static constexpr float PI_2_Inv = DirectX::XM_1DIV2PI;
static constexpr float PI_Div2 = DirectX::XM_PIDIV2;
static constexpr float PI_Div4 = DirectX::XM_PIDIV4;
static constexpr float Epsilon = std::numeric_limits<float>::epsilon();

template<typename T> 
inline T Clamp(const T& X, const T& Low, const T& High)
{
	return X < Low ? Low : (X > High ? High : X);
}

template<class T>
static constexpr inline T DivideAndRoundUp(T Dividend, T Divisor)
{
	return (Dividend + Divisor - 1) / Divisor;
}

template<class T>
static constexpr inline T DivideAndRoundDown(T Dividend, T Divisor)
{
	return Dividend / Divisor;
}

inline float DegreeToRadians(float Degree)
{
	return DirectX::XMConvertToRadians(Degree);
}

inline float RadiansToDegree(float Radians)
{
	return DirectX::XMConvertToDegrees(Radians);
}

/** Spreads bits to every other. */
static constexpr inline uint32_t MortonCode2(uint32_t X)
{
	X &= 0x0000ffff;
	X = (X ^ (X << 8)) & 0x00ff00ff;
	X = (X ^ (X << 4)) & 0x0f0f0f0f;
	X = (X ^ (X << 2)) & 0x33333333;
	X = (X ^ (X << 1)) & 0x55555555;
	return X;
}

static constexpr inline uint64_t MortonCode2_64(uint64_t X)
{
	X &= 0x00000000ffffffff;
	X = (X ^ (X << 16)) & 0x0000ffff0000ffff;
	X = (X ^ (X << 8)) & 0x00ff00ff00ff00ff;
	X = (X ^ (X << 4)) & 0x0f0f0f0f0f0f0f0f;
	X = (X ^ (X << 2)) & 0x3333333333333333;
	X = (X ^ (X << 1)) & 0x5555555555555555;
	return X;
}

/** Reverses MortonCode2. Compacts every other bit to the right. */
static constexpr inline uint32_t ReverseMortonCode2(uint32_t X)
{
	X &= 0x55555555;
	X = (X ^ (X >> 1)) & 0x33333333;
	X = (X ^ (X >> 2)) & 0x0f0f0f0f;
	X = (X ^ (X >> 4)) & 0x00ff00ff;
	X = (X ^ (X >> 8)) & 0x0000ffff;
	return X;
}

static constexpr inline uint64_t ReverseMortonCode2_64(uint64_t X)
{
	X &= 0x5555555555555555;
	X = (X ^ (X >> 1)) & 0x3333333333333333;
	X = (X ^ (X >> 2)) & 0x0f0f0f0f0f0f0f0f;
	X = (X ^ (X >> 4)) & 0x00ff00ff00ff00ff;
	X = (X ^ (X >> 8)) & 0x0000ffff0000ffff;
	X = (X ^ (X >> 16)) & 0x00000000ffffffff;
	return X;
}

/** Spreads bits to every 3rd. */
static constexpr inline uint32_t MortonCode3(uint32_t X)
{
	X &= 0x000003ff;
	X = (X ^ (X << 16)) & 0xff0000ff;
	X = (X ^ (X <<  8)) & 0x0300f00f;
	X = (X ^ (X <<  4)) & 0x030c30c3;
	X = (X ^ (X <<  2)) & 0x09249249;
	return X;
}

/** Reverses MortonCode3. Compacts every 3rd bit to the right. */
static constexpr inline uint32_t ReverseMortonCode3(uint32_t X)
{
	X &= 0x09249249;
	X = (X ^ (X >>  2)) & 0x030c30c3;
	X = (X ^ (X >>  4)) & 0x0300f00f;
	X = (X ^ (X >>  8)) & 0xff0000ff;
	X = (X ^ (X >> 16)) & 0x000003ff;
	return X;
}

NAMESPACE_END(Math)
