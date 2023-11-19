#include "Runtime/Core/Math/Intersect.h"

namespace Intersection
{
	bool8_t SphereWithSphere(const Math::Sphere& S0, const Math::Sphere& S1)
	{
		const float32_t CombinedRadius = S0.GetRadius() + S1.GetRadius();
		const Math::Vector3 CenterVector = S0.GetCenter() - S1.GetCenter();
		return CenterVector.LengthSq() < (CombinedRadius * CombinedRadius);
	}
}
