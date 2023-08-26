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
	std::shared_ptr<TAsset> FindOrImportAsset(StringType&& AssetPath, std::optional<Asset::Callbacks>& AssetLoadCallbacks = Asset::s_DefaultNullCallbacks, bool8_t Async = true)
	{
		auto UnifyPath = GetUnifyAssetPath(std::forward<StringType>(AssetPath));
		auto AssetIt = m_Assets.find(UnifyPath);
		if (AssetIt != m_Assets.end())
		{
			return Cast<TAsset, Asset>(AssetIt->second);
		}
		else
		{
			return Cast<TAsset>(ReimportAssetInternal(UnifyPath, AssetLoadCallbacks, Async));
		}
	}

	template<class StringType>
	void ReimportAsset(StringType&& AssetPath, std::optional<Asset::Callbacks>& AssetLoadCallbacks = Asset::s_DefaultNullCallbacks, bool8_t Async = true)
	{
		ReimportAssetInternal(GetUnifyAssetPath(std::forward<StringType>(AssetPath)), AssetLoadCallbacks, Async);
	}
private:
	template<class StringType>
	static std::filesystem::path GetUnifyAssetPath(StringType&& AssetPath, bool8_t Lowercase = false)
	{
		auto UnifyPath = std::filesystem::path(std::forward<StringType>(AssetPath)).make_preferred();
		return Lowercase ? std::filesystem::path(StringUtils::Lowercase(std::filesystem::path(UnifyPath).generic_string())) : UnifyPath;
	}

	void CreateAssetImporters();

	std::shared_ptr<Asset> ReimportAssetInternal(const std::filesystem::path& AssetPath, std::optional<Asset::Callbacks>& AssetLoadCallbacks, bool8_t Async);

	std::unordered_map<std::filesystem::path, std::shared_ptr<Asset>> m_Assets;
	std::vector<std::unique_ptr<IAssetImporter>> m_AssetImporters;
	bool8_t m_SupportAsyncLoad;
};

