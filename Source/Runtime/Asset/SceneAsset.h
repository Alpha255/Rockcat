#pragma once

#include "Asset/JsonAsset.h"

class SceneAsset : public JsonAsset<SceneAsset>
{
public:
	using Asset::Asset;
};