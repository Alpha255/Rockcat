#pragma once

#include "Scene/Components/ComponentBase.h"

class StaticMeshComponent : public ComponentBase
{
public:
	REGISTER_COMPONENT_ID(StaticMeshComponent)

	using ComponentBase::ComponentBase;

	inline void SetMesh(std::shared_ptr<class StaticMesh>& Mesh) { m_StaticMesh = Mesh; }
	inline const class StaticMesh& GetMesh() const
	{
		assert(m_StaticMesh);
		return *m_StaticMesh;
	}

	inline void SetMaterialProperty(std::shared_ptr<struct MaterialProperty>& Material){ m_Material = Material; }
	inline const struct MaterialProperty& GetMaterialProperty() const
	{
		assert(m_Material);
		return *m_Material;
	}

	inline struct MaterialProperty& GetMaterialProperty()
	{
		assert(m_Material);
		return *m_Material;
	}
protected:
private:
	std::shared_ptr<class StaticMesh> m_StaticMesh;
	std::shared_ptr<struct MaterialProperty> m_Material;
};