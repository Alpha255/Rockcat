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
	friend class Scene;

	SceneGraph Graph;
};

class AssimpScene2 : public Asset, public SceneGraph
{
public:
	using Asset::Asset;
};
