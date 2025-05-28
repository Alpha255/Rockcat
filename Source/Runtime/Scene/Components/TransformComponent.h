#pragma once

#include "Core/Math/Transform.h"
#include "Scene/Components/ComponentBase.h"

class TransformComponent : public ComponentBase
{
public:
	DECLARE_COMPONENT_ID(TransformComponent)

	inline void SetTransform(const Math::Transform& Trans)
	{
		m_Transform = Trans;
	}
private:
	Math::Transform m_Transform;
};