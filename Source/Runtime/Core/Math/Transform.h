#pragma once

#include "Core/Math/Matrix.h"
#include "Core/Math/Euler.h"
#include "Core/Math/Quaternion.h"

NAMESPACE_START(Math)

class Transform
{
public:
	Transform()
	{
		Identity();
	}

	Transform(const Vector3& Translation, const Vector3& Scalling, const Quaternion& Rotation)
		: m_Translation(Translation)
		, m_Scalling(Scalling)
		, m_Rotation(Rotation)
	{
	}

	inline void Identity()
	{
		m_Scalling = Vector3(1.0f);
		m_Translation = Vector3(0.0f);
		m_Rotation = Quaternion();
	}

	inline Transform& SetTranslation(const Vector3& Translation)
	{
		m_Translation = Translation;
		return *this;
	}

	inline Transform& SetTranslation(const float X, const float Y, const float Z)
	{
		m_Translation = Vector3(X, Y, Z);
		return *this;
	}

	inline Vector3 GetTranslation() const { return m_Translation; }

	inline Transform& SetScale(const Vector3& Scalling)
	{
		m_Scalling = Scalling;
		return *this;
	}

	inline Transform& SetScale(const float X, const float Y, const float Z)
	{
		m_Scalling = Vector3(X, Y, Z);
		return *this;
	}

	inline Transform& SetScale(const float Scalling)
	{
		m_Scalling = Vector3(Scalling);
		return *this;
	}

	inline Vector3 GetScalling() const { return m_Scalling; }

	inline Transform& SetRotationXAxis(const float Angle)
	{
		m_Rotation.RotationXAxis(Angle);
		return *this;
	}

	inline Transform& SetRotationYAxis(const float Angle)
	{
		m_Rotation.RotationYAxis(Angle);
		return *this;
	}

	inline Transform& SetRotationZAxis(const float Angle)
	{
		m_Rotation.RotationZAxis(Angle);
		return *this;
	}

	inline Transform& SetRotationAxis(const Vector3& Axis, const float Angle)
	{
		m_Rotation.RotationAxis(Axis, Angle);
		return *this;
	}

	inline Transform& SetRotation(const Vector3& Rotation)
	{
		m_Rotation.RotationXAxis(Rotation.x);
		m_Rotation.RotationYAxis(Rotation.y);
		m_Rotation.RotationZAxis(Rotation.z);
		return *this;
	}

	inline Transform& SetRotation(const float X, const float Y, const float Z)
	{
		m_Rotation.RotationXAxis(X);
		m_Rotation.RotationYAxis(Y);
		m_Rotation.RotationZAxis(Z);
		return *this;
	}

	inline Quaternion GetRotation() const { return m_Rotation; }

	inline Matrix GetMatrix() const
	{
		return Matrix::Scaling(m_Scalling) * m_Rotation.GetRotationMatrix() * Matrix::Translation(m_Translation);
	}

	inline void SetMatrix(const Matrix& InMatrix)
	{
		InMatrix.Decompose(m_Translation, m_Scalling, m_Rotation);
	}

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_NVP(m_Translation),
			CEREAL_NVP(m_Scalling),
			CEREAL_NVP(m_Rotation)
		);
	}
protected:
private:
	Vector3 m_Translation;
	Vector3 m_Scalling;
	Quaternion m_Rotation;
};

NAMESPACE_END(Math)
