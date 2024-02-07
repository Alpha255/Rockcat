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

NAMESPACE_END(Math)
