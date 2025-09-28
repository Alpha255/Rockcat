#pragma once

#include "Asset/Asset.h"

class DDSTextureLoader : public AssetLoader
{
public:
	DDSTextureLoader()
		: AssetLoader({ AssetType{"DirectDraw Surface", ".dds"} })
	{
	}
};