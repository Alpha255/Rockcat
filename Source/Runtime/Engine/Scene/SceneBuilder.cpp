#pragma once

#include "Runtime/Engine/Scene/SceneBuilder.h"

void SceneBuilder::Merge(Scene& Target, const Scene& Other)
{
}

void SceneBuilder::MergeSceneGraph(SceneGraph& Target, const SceneGraph& Other)
{
}

std::shared_ptr<SceneGraph> SceneBuilder::MergeSceneGraph(const SceneGraph& Left, const SceneGraph& Right)
{
	return std::shared_ptr<SceneGraph>();
}
