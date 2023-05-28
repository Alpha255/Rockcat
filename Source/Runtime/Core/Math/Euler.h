#pragma once

#include "Runtime/Core/Math/Vector4.h"

NAMESPACE_START(Math)

class Euler : public Vector4
{
public:
	void Set(float32_t Roll, float32_t Pitch, float32_t Yaw);
protected:
private:
};

NAMESPACE_END(Math)