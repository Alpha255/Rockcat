#pragma once

#include "Runtime/Engine/Asset/Asset.h"
#include "Runtime/Core/StringUtils.h"

DECLARE_OBJECT_ID(Asset, uint32_t)

class AssetDatabase
{
public:
	AssetDatabase();

	~AssetDatabase();

	static AssetDatabase& Get();

	template<class TAsset, class StringType>
	TAsset* FindOrLoadAsset(StringType&& AssetPath, std::optional<Asset::Callbacks>& AssetLoadCallbacks = Asset::s_DefaultNullCallbacks)
	{
		auto GenericAssetPath = GetGenericAssetPath(std::forward<StringType>(AssetPath));
		auto AssetIt = m_Assets.find(GenericAssetPath);
		if (AssetIt != m_Assets.end())
		{
			return Cast<TAsset>(AssetIt->second.get());
		}
		else
		{
			return Cast<TAsset>(ReimportAssetInternal(GenericAssetPath, AssetLoadCallbacks));
		}
	}

	template<class StringType>
	void ReimportAsset(StringType&& AssetPath)
	{
		return ReimportAssetInternal(GetGenericAssetPath(std::forward<StringType>(AssetPath)), std::nullopt);
	}
private:
	template<class StringType>
	static std::filesystem::path GetGenericAssetPath(StringType&& AssetPath)
	{
		return std::filesystem::path(
			StringUtils::Replace(
				StringUtils::Lowercase(
					std::filesystem::path(std::forward<StringType>(AssetPath)).generic_string()), "/", "\\"));
	}

	void CreateAssetImporters();

	Asset* ReimportAssetInternal(const std::filesystem::path& AssetPath, std::optional<Asset::Callbacks>& AssetLoadCallbacks);

	std::unordered_map<std::filesystem::path, std::shared_ptr<Asset>> m_Assets;
	std::vector<std::unique_ptr<IAssetImporter>> m_AssetImporters;
	bool8_t m_AsyncLoadAssets;
};

