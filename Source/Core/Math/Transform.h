#pragma once

#include "Core/Math/Matrix.h"
#include "Core/Math/Euler.h"
#include "Core/Math/Quaternion.h"

NAMESPACE_START(Gear)
NAMESPACE_START(Math)

class Transform
{
public:
	inline void Reset()
	{
		m_Scaling = Vector3(0.0f, 0.0f, 0.0f);
		m_Translation = Vector3(0.0f, 0.0f, 0.0f);
		m_Rotation = Vector4(0.0f, 0.0f, 0.0f, 0.0f);
		m_Matrix.Identity();
	}

	inline Matrix GetMatrix()
	{
		Build();
		return m_Matrix;
	}

	inline void Scale(const Vector3& Factor)
	{
		m_Scaling *= Factor;
	}

	inline void Scale(const float32_t X, const float32_t Y, const float32_t Z)
	{
		m_Scaling *= Vector3(X, Y, Z);
	}

	inline void Scale(const float32_t Factor)
	{
		m_Scaling *= Vector3(Factor, Factor, Factor);
	}

	inline void Translate(const Vector3& Translation)
	{
		m_Translation += Translation;
	}

	inline void Translate(const float32_t X, const float32_t Y, const float32_t Z)
	{
		m_Translation += Vector3(X, Y, Z);
	}

	inline void RotateXAxis(const float32_t Angle)
	{
		m_Rotation += Vector4(1.0f, 0.0f, 0.0f, Angle);
	}

	inline void RotateYAxis(const float32_t Angle)
	{
		m_Rotation += Vector4(0.0f, 1.0f, 0.0f, Angle);
	}

	inline void RotateZAxis(const float32_t Angle)
	{
		m_Rotation += Vector4(0.0f, 1.0f, 0.0f, Angle);
	}

	inline void RotateAxis(const Vector3& Axis, const float32_t Angle)
	{
		m_Rotation += Vector4(Axis, Angle);
	}

	inline Matrix TransposedMatrix()
	{
		Build();
		return Matrix::Transpose(m_Matrix);
	}

	inline Matrix InversedMatrix()
	{
		Build();
		return Matrix::Inverse(m_Matrix);
	}

	inline Matrix InverseTransposedMatrix()
	{
		Build();
		return Matrix::InverseTranspose(m_Matrix);
	}
protected:
	void Build()
	{
		Reset();

		m_Matrix = 
			Matrix::Translation(m_Translation) * 
			Matrix::Rotation(m_Rotation.x, m_Rotation.y, m_Rotation.z, m_Rotation.w) * 
			Matrix::Scaling(m_Scaling);
	}
private:
	Matrix m_Matrix;
	Vector3 m_Scaling;
	Vector3 m_Translation;
	Vector4 m_Rotation;
};

NAMESPACE_END(Math)
NAMESPACE_END(Gear)
