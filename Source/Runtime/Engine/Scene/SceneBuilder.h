#pragma once

#include "Runtime/Engine/Scene/Scene.h"

class SceneBuilder
{
public:
	void Merge(Scene& Target, const Scene& Other);

	void MergeSceneGraph(SceneGraph& Target, const SceneGraph& Other);

	std::shared_ptr<SceneGraph> MergeSceneGraph(const SceneGraph& Left, const SceneGraph& Right);
protected:
private:
};

