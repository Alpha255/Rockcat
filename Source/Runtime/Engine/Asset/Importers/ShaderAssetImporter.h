#pragma once

#include "Runtime/Engine/Asset/ShaderAsset.h"

class ShaderAssetImporter : public IAssetImporter
{
public:
	ShaderAssetImporter()
		: IAssetImporter({".vert", ".frag", ".geo", ".comp", ".hlsl"})
	{
	}

	std::shared_ptr<Asset> CreateAsset(const std::filesystem::path& AssetPath) override final { return std::make_shared<ShaderAsset>(AssetPath); }

	bool8_t Reimport(Asset& InAsset) override final
	{
		return false;
	}
};
