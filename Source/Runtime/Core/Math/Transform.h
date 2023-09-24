#pragma once

#include "Runtime/Core/Math/Matrix.h"
#include "Runtime/Core/Math/Euler.h"
#include "Runtime/Core/Math/Quaternion.h"

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

	inline Transform& SetTranslation(const float32_t X, const float32_t Y, const float32_t Z)
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

	inline Transform& SetScale(const float32_t X, const float32_t Y, const float32_t Z)
	{
		m_Scalling = Vector3(X, Y, Z);
		return *this;
	}

	inline Transform& SetScale(const float32_t Scalling)
	{
		m_Scalling = Vector3(Scalling);
		return *this;
	}

	inline Vector3 GetScalling() const { return m_Scalling; }

	inline Transform& SetRotationXAxis(const float32_t Angle)
	{
		m_Rotation.RotationXAxis(Angle);
		return *this;
	}

	inline Transform& SetRotationYAxis(const float32_t Angle)
	{
		m_Rotation.RotationYAxis(Angle);
		return *this;
	}

	inline Transform& SetRotationZAxis(const float32_t Angle)
	{
		m_Rotation.RotationZAxis(Angle);
		return *this;
	}

	inline Transform& SetRotationAxis(const Vector3& Axis, const float32_t Angle)
	{
		m_Rotation.RotationAxis(Axis, Angle);
		return *this;
	}

	inline Quaternion GetRotation() const { return m_Rotation; }

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
