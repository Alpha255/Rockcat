#include "Runtime/Core/Math/AABB.h"

NAMESPACE_START(Math)

AABB AABB::CreateFromVertices(const std::vector<Vector3>& Vertices)
{
	Vector3 Min, Max;
	for (uint32_t i = 0u; i < Vertices.size(); ++i)
	{
		Min = Math::Min(Min, Vertices[i]);
		Max = Math::Max(Max, Vertices[i]);
	}

	return AABB(Min, Max);
}

NAMESPACE_END(Math)
