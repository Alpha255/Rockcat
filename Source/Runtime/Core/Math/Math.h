#pragma once

#include "Runtime/Core/Math/AABB.h"
#include "Runtime/Core/Math/Color.h"
#include "Runtime/Core/Math/Euler.h"
#include "Runtime/Core/Math/Frustum.h"
#include "Runtime/Core/Math/Matrix.h"
#include "Runtime/Core/Math/OBB.h"
#include "Runtime/Core/Math/Plane.h"
#include "Runtime/Core/Math/Quaternion.h"
#include "Runtime/Core/Math/Rect.h"
#include "Runtime/Core/Math/Sphere.h"
#include "Runtime/Core/Math/Transform.h"

NAMESPACE_START(Gear)
NAMESPACE_START(Math)

static constexpr float32_t PI = DirectX::XM_PI;
static constexpr float32_t PI_2 = DirectX::XM_2PI;
static constexpr float32_t PI_Inv = DirectX::XM_1DIVPI;
static constexpr float32_t PI_2_Inv = DirectX::XM_1DIV2PI;
static constexpr float32_t PI_Div2 = DirectX::XM_PIDIV2;
static constexpr float32_t PI_Div4 = DirectX::XM_PIDIV4;
static constexpr float32_t Epsilon = std::numeric_limits<float32_t>().epsilon();

template<typename T> inline T Clamp(const T& X, const T& Low, const T& High)
{
	return X < Low ? Low : (X > High ? High : X);
}

inline float32_t DegreeToRadians(float32_t Degree)
{
	return DirectX::XMConvertToRadians(Degree);
}

inline float32_t RadiansToDegree(float32_t Radians)
{
	return DirectX::XMConvertToDegrees(Radians);
}

NAMESPACE_END(Math)
NAMESPACE_END(Gear)