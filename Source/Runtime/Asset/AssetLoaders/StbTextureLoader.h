#pragma once

#include "Asset/Asset.h"

class StbTextureLoader : public AssetLoader
{
public:
	StbTextureLoader();

	bool Load(Asset& InAsset, const AssetType& Type) override final;
protected:
	std::shared_ptr<Asset> CreateAssetImpl(const std::filesystem::path& Path) override final;
};