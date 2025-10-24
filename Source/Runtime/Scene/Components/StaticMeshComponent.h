#pragma once

#include "Scene/Components/ComponentBase.h"

class StaticMeshComponent : public ComponentBase
{
public:
	DECLARE_COMPONENT_ID(StaticMeshComponent)

	using ComponentBase::ComponentBase;

	inline void SetMesh(std::shared_ptr<class StaticMesh> Mesh)
	{
		m_StaticMesh = Mesh;
	}

	inline void SetMaterial(std::shared_ptr<struct MaterialProperty> Material)
	{
		m_Material = Material;
	}
private:
	std::shared_ptr<class StaticMesh> m_StaticMesh;
	std::shared_ptr<struct MaterialProperty> m_Material;
};