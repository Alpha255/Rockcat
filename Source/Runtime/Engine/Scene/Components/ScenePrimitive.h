#pragma once

#include "Engine/Scene/Components/StaticMesh.h"
#include "Engine/Scene/SceneGraph.h"

struct PrimitiveProperty
{

};

struct PrimitiveBatch : public PrimitiveProperty
{
	std::vector<SceneGraph::NodeID> Nodes;

	inline uint32_t GetNumInstance() const { return static_cast<uint32_t>(Nodes.size()); }
};
