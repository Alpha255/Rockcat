#pragma once

#include "Core/Math/Transform.h"
#include "Scene/Components/ComponentBase.h"

class TransformComponent : public ComponentBase
{
public:
	REGISTER_COMPONENT_ID(TransformComponent);

	using ComponentBase::ComponentBase;

	inline TransformComponent& SetTranslation(const Math::Vector3& Translation)
	{
		m_Transform.SetTranslation(Translation);
		return *this;
	}

	inline TransformComponent& SetTranslation(const float X, const float Y, const float Z)
	{
		m_Transform.SetTranslation(X, Y, Z);
		return *this;
	}

	inline TransformComponent& SetTranslationX(const float X)
	{
		m_Transform.SetTranslationX(X);
		return *this;
	}

	inline TransformComponent& SetTranslationY(const float Y)
	{
		m_Transform.SetTranslationY(Y);
		return *this;
	}

	inline TransformComponent& SetTranslationZ(const float Z)
	{
		m_Transform.SetTranslationZ(Z);
		return *this;
	}

	inline TransformComponent& SetRotationX(const float Angle)
	{
		m_Transform.SetRotationXAxis(Angle);
		return *this;
	}

	inline TransformComponent& SetRotationY(const float Angle)
	{
		m_Transform.SetRotationYAxis(Angle);
		return *this;
	}

	inline TransformComponent& SetRotationZ(const float Angle)
	{
		m_Transform.SetRotationZAxis(Angle);
		return *this;
	}

	inline TransformComponent& SetRotation(const Math::Vector3& Rotation)
	{
		m_Transform.SetRotation(Rotation);
		return *this;
	}

	inline TransformComponent& SetRotation(const float X, const float Y, const float Z)
	{
		m_Transform.SetRotation(X, Y, Z);
		return *this;
	}

	inline TransformComponent& SetRotationAxis(const Math::Vector3& Axis, const float Angle)
	{
		m_Transform.SetRotationAxis(Axis, Angle);
		return *this;
	}

	inline TransformComponent& SetScale(const Math::Vector3& Scalling)
	{
		m_Transform.SetScale(Scalling);
		return *this;
	}

	inline TransformComponent& SetScale(const float X, const float Y, const float Z)
	{
		m_Transform.SetScale(X, Y, Z);
		return *this;
	}

	inline TransformComponent& SetScale(const float Scalling)
	{
		m_Transform.SetScale(Scalling);
		return *this;
	}

	inline void SetTransform(const Math::Transform& InTransform) { m_Transform = InTransform; }

	inline Math::Vector3 GetTranslation() const { return m_Transform.GetTranslation(); }
	inline Math::Vector3 GetScalling() const { return m_Transform.GetScalling(); }
	inline Math::Quaternion GetRotation() const { return m_Transform.GetRotation(); }
	inline void Reset() { m_Transform.Identity(); }

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_NVP(m_Transform)
		);
	}
private:
	Math::Transform m_Transform;
};

CEREAL_REGISTER_TYPE(TransformComponent);
CEREAL_REGISTER_POLYMORPHIC_RELATION(ComponentBase, TransformComponent);