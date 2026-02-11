#pragma once

#include "Asset/Asset.h"

class TextureLoader : public AssetLoader
{
public:
	TextureLoader();

	bool Load(Asset& Target) override final;
protected:
	static constexpr std::string_view GetDDSExtension() { return ".dds"; }

	inline bool IsDDSTexture(const std::string& Extension)
	{
		return _stricmp(Extension.c_str(), GetDDSExtension().data());
	}

	std::shared_ptr<Asset> CreateAsset(const std::filesystem::path& Path) override final;

	bool LoadStb(class Texture& Image);
	bool LoadDDS(class Texture& Image);
};
