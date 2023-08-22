#pragma once

#include "Runtime/Engine/Asset/Asset.h"
#include "Runtime/Core/StringUtils.h"

DECLARE_OBJECT_ID(Asset, uint32_t)

class AssetDatabase
{
public:
	AssetDatabase();

	~AssetDatabase() = default;

	static AssetDatabase& Get();

	template<class TAsset, class StringType>
	TAsset* FindOrImportAsset(StringType&& AssetPath, std::optional<Asset::Callbacks>& AssetLoadCallbacks = Asset::s_DefaultNullCallbacks, bool8_t Async = true)
	{
		auto GenericAssetPath = GetGenericAssetPath(std::forward<StringType>(AssetPath));
		auto AssetIt = m_Assets.find(GenericAssetPath);
		if (AssetIt != m_Assets.end())
		{
			return Cast<TAsset>(AssetIt->second.get());
		}
		else
		{
			return Cast<TAsset>(ReimportAssetInternal(GenericAssetPath, AssetLoadCallbacks, Async));
		}
	}

	template<class StringType>
	void ReimportAsset(StringType&& AssetPath, std::optional<Asset::Callbacks>& AssetLoadCallbacks = Asset::s_DefaultNullCallbacks, bool8_t Async = true)
	{
		return ReimportAssetInternal(GetGenericAssetPath(std::forward<StringType>(AssetPath)), AssetLoadCallbacks, Async);
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

	Asset* ReimportAssetInternal(const std::filesystem::path& AssetPath, std::optional<Asset::Callbacks>& AssetLoadCallbacks, bool8_t Async);

	std::unordered_map<std::filesystem::path, std::shared_ptr<Asset>> m_Assets;
	std::vector<std::unique_ptr<IAssetImporter>> m_AssetImporters;
	bool8_t m_SupportAsyncLoad;
};

