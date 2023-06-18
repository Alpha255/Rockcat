#pragma once

#include "Runtime/Engine/Asset/ImageAsset.h"

class DDSImageImporter : public IAssetImporter
{
public:
	DDSImageImporter()
		: IAssetImporter({".dds"})
	{
	}

	std::shared_ptr<Asset> CreateAsset(const std::filesystem::path& AssetPath) override final { return std::make_shared<ImageAsset>(AssetPath); }

	void Reimport(Asset& InAsset) override final
	{
	}
};
