#pragma once

#include "Runtime/Engine/Asset/ShaderAsset.h"

class ShaderAssetImporter : public IAssetImporter
{
public:
	ShaderAssetImporter()
		: IAssetImporter({".vert", ".frag", ".geo", ".comp", ".hlsl"})
	{
	}

	std::shared_ptr<Asset> CreateAsset(const char8_t* AssetPath) override final { return std::make_shared<ShaderAsset>(AssetPath); }

	void Reimport(Asset& InAsset) override final
	{
	}
};
