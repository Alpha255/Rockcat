#pragma once

#include "Runtime/Engine/Scene/Scene.h"

class SceneBuilder
{
public:
	static std::unique_ptr<Scene> BuildSceneFromSceneAsset(const char8_t* AssetName);
protected:
private:
};

