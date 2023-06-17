#pragma once

#include "Runtime/Engine/Asset/ImageAsset.h"

class StbImageImporter : public IAssetImporter
{
public:
	StbImageImporter()
		: IAssetImporter({".jpeg", ".jpg", ".png", ".bmp", ".tga", ".psd", ".hdr"})
	{
	}

	std::shared_ptr<Asset> CreateAsset(const char8_t* AssetPath) override final { return std::make_shared<ImageAsset>(AssetPath); }

	void Reimport(Asset& InAsset) override final
	{
	}
};
