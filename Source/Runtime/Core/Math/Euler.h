#pragma once

#include "Core/Math/Vector4.h"

NAMESPACE_START(Math)

class Euler : public Vector4
{
public:
	void Set(float Roll, float Pitch, float Yaw);
protected:
private:
};

NAMESPACE_END(Math)
