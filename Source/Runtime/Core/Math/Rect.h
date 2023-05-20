#pragma once

#include "Runtime/Core/Math/Vector4.h"

NAMESPACE_START(Math)

class Rect : public Vector4
{
public:
	Rect()
		: Vector4(0.0f, 0.0f, 0.0f, 0.0f)
	{
	}

	Rect(float32_t Left, float32_t Top, float32_t Right, float32_t Bottom)
		: Vector4(Left, Top, Right, Bottom)
	{
	}

	Rect(const Vector4& Other)
		: Vector4(Other)
	{
	}

	inline float32_t Width()
	{
		return z - x;
	}

	inline float32_t Height()
	{
		return w - y;
	}

	bool8_t IsIntersect(const Rect& Other);
protected:
private:
};

NAMESPACE_END(Math)
