#pragma once

#include "Engine/Asset/Asset.h"
#include "Core/StringUtils.h"
#include "Core/Module.h"

DECLARE_OBJECT_ID(Asset, uint32_t)

class AssetDatabase : public IService<AssetDatabase>
{
public:
	AssetDatabase();

	void OnShutdown() override final;

	template<class TAsset, class T>
	std::shared_ptr<TAsset> FindOrImportAsset(T&& AssetPath, std::optional<Asset::Callbacks>& AssetLoadCallbacks = Asset::s_DefaultNullCallbacks, bool Async = true)
	{
		auto UnifyPath = GetUnifyAssetPath(std::forward<T>(AssetPath));
		auto AssetIt = m_Assets.find(UnifyPath);
		if (AssetIt != m_Assets.end())
		{
			return Cast<TAsset, Asset>(AssetIt->second);
		}
		else
		{
			std::shared_ptr<Asset> EmptyAsset;
			return Cast<TAsset>(ReimportAssetImpl(EmptyAsset, UnifyPath, AssetLoadCallbacks, Async));
		}
	}

	void FindOrImportAsset(std::shared_ptr<Asset>& TargetAsset, std::optional<Asset::Callbacks>& AssetLoadCallbacks = Asset::s_DefaultNullCallbacks, bool Async = true)
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
			ReimportAssetImpl(TargetAsset, UnifyPath, AssetLoadCallbacks, Async);
		}
	}

	template<class T>
	void ReimportAsset(T&& AssetPath, std::optional<Asset::Callbacks>& AssetLoadCallbacks = Asset::s_DefaultNullCallbacks, bool Async = true)
	{
		ReimportAssetImpl(nullptr, GetUnifyAssetPath(std::forward<T>(AssetPath)), AssetLoadCallbacks, Async);
	}

	void ReimportAsset(std::shared_ptr<Asset>& TargetAsset, std::optional<Asset::Callbacks>& AssetLoadCallbacks = Asset::s_DefaultNullCallbacks, bool Async = true)
	{
		assert(TargetAsset);

		ReimportAssetImpl(TargetAsset, GetUnifyAssetPath(TargetAsset->GetPath()), AssetLoadCallbacks, Async);
	}
private:
	template<class T>
	static std::filesystem::path GetUnifyAssetPath(T&& AssetPath, bool Lowercase = false)
	{
		auto UnifyPath = std::filesystem::path(std::forward<T>(AssetPath)).make_preferred();
		return Lowercase ? std::filesystem::path(StringUtils::Lowercase(UnifyPath.generic_string())) : UnifyPath;
	}

	void CreateAssetImporters();

	std::shared_ptr<Asset> ReimportAssetImpl(
		std::shared_ptr<Asset>& TargetAsset,
		const std::filesystem::path& AssetPath, 
		std::optional<Asset::Callbacks>& AssetLoadCallbacks, 
		bool Async);

	std::unordered_map<std::filesystem::path, std::shared_ptr<Asset>> m_Assets;
	std::vector<std::unique_ptr<IAssetImporter>> m_AssetImporters;
	bool m_SupportAsyncLoad;
};

