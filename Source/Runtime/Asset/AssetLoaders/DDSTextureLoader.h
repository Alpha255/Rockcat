#pragma once

#include "Asset/Asset.h"

class DDSTextureLoader : public AssetLoader
{
public:
	DDSTextureLoader();

	bool Load(Asset& InAsset) override final;
protected:
	std::shared_ptr<Asset> CreateAsset(const std::filesystem::path& Path) override final;
};