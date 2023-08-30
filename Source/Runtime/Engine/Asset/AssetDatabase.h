#pragma once

#include "Runtime/Engine/Asset/Asset.h"
#include "Runtime/Core/StringUtils.h"
#include "Runtime/Core/Module.h"

DECLARE_OBJECT_ID(Asset, uint32_t)

class AssetDatabase : public IService<AssetDatabase>
{
public:
	AssetDatabase();

	void OnShutdown() override final;

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
			return Cast<TAsset>(ReimportAssetImpl(nullptr, UnifyPath, AssetLoadCallbacks, Async));
		}
	}

	void FindOrImportAsset(std::shared_ptr<Asset>& TargetAsset, std::optional<Asset::Callbacks>& AssetLoadCallbacks = Asset::s_DefaultNullCallbacks, bool8_t Async = true)
	{
		assert(TargetAsset);

		auto UnifyPath = GetUnifyAssetPath(TargetAsset->GetPath());
		auto AssetIt = m_Assets.find(UnifyPath);
		if (AssetIt != m_Assets.end())
		{
			TargetAsset = AssetIt->second;
		}
		else
		{
			ReimportAssetImpl(TargetAsset.get(), UnifyPath, AssetLoadCallbacks, Async);
		}
	}

	template<class StringType>
	void ReimportAsset(StringType&& AssetPath, std::optional<Asset::Callbacks>& AssetLoadCallbacks = Asset::s_DefaultNullCallbacks, bool8_t Async = true)
	{
		ReimportAssetImpl(nullptr, GetUnifyAssetPath(std::forward<StringType>(AssetPath)), AssetLoadCallbacks, Async);
	}

	void ReimportAsset(std::shared_ptr<Asset>& TargetAsset, std::optional<Asset::Callbacks>& AssetLoadCallbacks = Asset::s_DefaultNullCallbacks, bool8_t Async = true)
	{
		assert(TargetAsset);

		ReimportAssetImpl(TargetAsset.get(), GetUnifyAssetPath(TargetAsset->GetPath()), AssetLoadCallbacks, Async);
	}
private:
	template<class StringType>
	static std::filesystem::path GetUnifyAssetPath(StringType&& AssetPath, bool8_t Lowercase = false)
	{
		auto UnifyPath = std::filesystem::path(std::forward<StringType>(AssetPath)).make_preferred();
		return Lowercase ? std::filesystem::path(StringUtils::Lowercase(UnifyPath.generic_string())) : UnifyPath;
	}

	void CreateAssetImporters();

	std::shared_ptr<Asset> ReimportAssetImpl(
		Asset* TargetAsset, 
		const std::filesystem::path& AssetPath, 
		std::optional<Asset::Callbacks>& AssetLoadCallbacks, 
		bool8_t Async);

	std::unordered_map<std::filesystem::path, std::shared_ptr<Asset>> m_Assets;
	std::vector<std::unique_ptr<IAssetImporter>> m_AssetImporters;
	bool8_t m_SupportAsyncLoad;
};

