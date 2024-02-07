#pragma once

#include "Core/Math/Vector4.h"

NAMESPACE_START(Math)

class Rect : public Vector4
{
public:
	Rect()
		: Vector4(0.0f, 0.0f, 0.0f, 0.0f)
	{
	}

	Rect(float Left, float Top, float Right, float Bottom)
		: Vector4(Left, Top, Right, Bottom)
	{
	}

	Rect(const Vector4& Other)
		: Vector4(Other)
	{
	}

	inline float Width()
	{
		return z - x;
	}

	inline float Height()
	{
		return w - y;
	}

	bool IsIntersect(const Rect& Other);
protected:
private:
};

NAMESPACE_END(Math)
