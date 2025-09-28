#pragma once

#include "Core/Math/Transform.h"
#include "Asset/Material.h"
#include "Scene/SceneGraph.h"
#include "Scene/Components/Camera.h"
#include "Scene/Components/StaticMesh.h"
#include "Paths.h"

struct AssimpScene : public Asset
{
	using Asset::Asset;

	struct DataIndex
	{
		uint32_t Mesh = ~0u;
		uint32_t Material = ~0u;
		uint32_t Transfrom = ~0u;
	};

	SceneGraph Graph;

	std::vector<std::shared_ptr<StaticMesh>> StaticMeshes;
	std::vector<std::shared_ptr<MaterialProperty>> MaterialProperties;
	std::vector<Math::Transform> Transforms;

	std::unordered_map<EntityID, DataIndex> EntityDataIndices;
};
