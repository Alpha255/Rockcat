#pragma once

#include "Scene/Components/ComponentBase.h"

class StaticMeshComponent : public ComponentBase
{
public:
	REGISTER_COMPONENT_ID(StaticMeshComponent)

	using ComponentBase::ComponentBase;

	inline void SetMesh(std::shared_ptr<class StaticMesh> Mesh) { m_StaticMesh = Mesh; }
	const class StaticMesh* GetMesh() const { return m_StaticMesh.get(); }

	inline void SetMaterialProperty(std::shared_ptr<struct MaterialProperty> Material) { m_Material = Material; }
	const struct MaterialProperty* GetMaterialProperty() const { return m_Material.get(); }
private:
	std::shared_ptr<class StaticMesh> m_StaticMesh;
	std::shared_ptr<struct MaterialProperty> m_Material;
};