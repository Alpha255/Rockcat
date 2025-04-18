#pragma once

#include "Core/Math/Plane.h"
#include "Core/Math/Matrix.h"
#include "Core/Math/AABB.h"

NAMESPACE_START(Math)

class Frustum
{
public:
	enum EPlane
	{
		Near,
		Far,
		Left,
		Right,
		Top,
		Bottom,
		Counts
	};

	Frustum() = default;

	Frustum(const Matrix& Viewport, bool Reverse)
	{
		Vector4 Rows[4] = { Viewport.Row(0), Viewport.Row(1), Viewport.Row(2), Viewport.Row(3) };
		
		m_Planes[EPlane::Near] = Plane(Vector3(-Rows[0].z, -Rows[1].z, -Rows[2].z), Rows[3].z);
		m_Planes[EPlane::Far] = Plane(Vector3(-Rows[0].w + Rows[0].z, -Rows[1].w + Rows[1].z, -Rows[2].w + Rows[2].z), Rows[3].w - Rows[3].z);
		
		if (Reverse)
		{
			std::swap(m_Planes[EPlane::Near], m_Planes[EPlane::Far]);
		}

		m_Planes[EPlane::Left] = Plane(Vector3(-Rows[0].w - Rows[0].x, -Rows[1].w - Rows[1].x, -Rows[2].w - Rows[2].x), Rows[3].w + Rows[3].x);
		m_Planes[EPlane::Right] = Plane(Vector3(-Rows[0].w + Rows[0].x, -Rows[1].w + Rows[1].x, -Rows[2].w + Rows[2].x), Rows[3].w - Rows[3].x);

		m_Planes[EPlane::Top] = Plane(Vector3(-Rows[0].w + Rows[0].y, -Rows[1].w + Rows[1].y, -Rows[2].w + Rows[2].y), Rows[3].w - Rows[3].y);
		m_Planes[EPlane::Bottom] = Plane(Vector3(-Rows[0].w - Rows[0].y, -Rows[1].w - Rows[1].y, -Rows[2].w - Rows[2].y), Rows[3].w + Rows[3].y);

		Normalize();
	}

	void Normalize()
	{
		for (uint32_t Index = EPlane::Near; Index < EPlane::Counts; ++Index)
		{
			m_Planes[Index].Normalize();
		}
	}

	bool IntersectsWith(const Vector3& Point)
	{
		for (uint32_t Index = EPlane::Near; Index < EPlane::Counts; ++Index)
		{
			float Distance = Dot(m_Planes[Index].Normal(), Point);
			if (Distance > m_Planes[Index].Distance())
			{
				return false;
			}
		}

		return true;
	}

	bool IntersectsWith(const AABB& Box)
	{
		for (uint32_t Index = EPlane::Near; Index < EPlane::Counts; ++Index)
		{
			Vector3 Normal = m_Planes[Index].Normal();
			Vector3 Min = Box.GetMin();
			Vector3 Max = Box.GetMax();

			float X = Normal.x > 0 ? Min.x : Max.x;
			float Y = Normal.y > 0 ? Min.y : Max.y;
			float Z = Normal.z > 0 ? Min.z : Max.z;

			float Distance = 
				Normal.x * X + 
				Normal.y + Y + 
				Normal.z * Z - 
				m_Planes[Index].Distance();
			if (Distance > 0)
			{
				return false;
			}
		}

		return true;
	}
protected:
private:
	Plane m_Planes[EPlane::Counts];
};

NAMESPACE_END(Math)
