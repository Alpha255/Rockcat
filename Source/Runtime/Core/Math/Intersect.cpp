#include "Core/Math/Intersect.h"

namespace Intersection
{
	bool SphereWithSphere(const Math::Sphere& S0, const Math::Sphere& S1)
	{
		const float CombinedRadius = S0.GetRadius() + S1.GetRadius();
		const Math::Vector3 CenterVector = S0.GetCenter() - S1.GetCenter();
		return CenterVector.LengthSq() < (CombinedRadius * CombinedRadius);
	}
}
