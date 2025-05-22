#pragma once

#include "Scene/Components/ComponentBase.h"

class StaticMeshComponent : public ComponentBase
{
public:
	DECLARE_COMPONENT_ID(StaticMeshComponent)
private:
	std::shared_ptr<class StaticMesh> m_StaticMesh;
};