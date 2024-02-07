#include "Engine/Scene/Components/StaticMesh.h"

RHIInputLayoutCreateInfo MeshData::GetInputLayoutCreateInfo() const
{
	return RHIInputLayoutCreateInfo();
}

StaticMesh::StaticMesh(const MeshData& Data, MaterialID Material)
	: MeshProperty(Data)
	, m_Material(Material)
{
}
