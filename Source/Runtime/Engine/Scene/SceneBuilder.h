#pragma once

#include "Runtime/Engine/Scene/Scene.h"

class SceneBuilder
{
public:
	static void Merge(Scene& Target, const Scene& Other);

	static void MergeSceneGraph(SceneGraph& Target, const SceneGraph& Other);

	static std::shared_ptr<SceneGraph> MergeSceneGraph(const SceneGraph& Left, const SceneGraph& Right);
protected:
private:
};

