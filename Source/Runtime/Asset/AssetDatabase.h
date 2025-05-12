#pragma once

#include "Core/StringUtils.h"
#include "Core/Module.h"
#include "Asset/Asset.h"
#include "Async/Task.h"

class AssetDatabase : public IService<AssetDatabase>
{
public:
	void Initialize() override final;
	void Finalize() override final;

	template<class TAsset>
	std::shared_ptr<TAsset> GetOrReimportAsset(const std::filesystem::path& Path, std::optional<Asset::Callbacks>& AssetLoadCallbacks = Asset::s_DefaultNullCallbacks, bool Async = true)
	{
		auto UnifyPath = GetUnifyAssetPath(Path);
		auto AssetTaskIt = m_AssetLoadTasks.find(UnifyPath);
		if (AssetTaskIt != m_AssetLoadTasks.end())
		{
			return Cast<TAsset, Asset>(AssetTaskIt->second->Asset);
		}
		else
		{
			std::shared_ptr<Asset> EmptyAsset;
			return Cast<TAsset>(ReimportAssetImpl(EmptyAsset, UnifyPath, AssetLoadCallbacks, Async));
		}
	}

	void GetOrReimportAsset(std::shared_ptr<Asset>& TargetAsset, std::optional<Asset::Callbacks>& AssetLoadCallbacks = Asset::s_DefaultNullCallbacks, bool Async = true)
	{
		assert(TargetAsset);

		auto UnifyPath = GetUnifyAssetPath(TargetAsset->GetPath());
		auto AssetTaskIt = m_AssetLoadTasks.find(UnifyPath);
		if (AssetTaskIt != m_AssetLoadTasks.end())
		{
			TargetAsset = AssetTaskIt->second->Asset;
		}
		else
		{
			ReimportAssetImpl(TargetAsset, std::move(UnifyPath), AssetLoadCallbacks, Async);
		}
	}

	void ReimportAsset(const std::filesystem::path& Path, std::optional<Asset::Callbacks>& AssetLoadCallbacks = Asset::s_DefaultNullCallbacks, bool Async = true)
	{
		std::shared_ptr<Asset> EmptyAsset;
		ReimportAssetImpl(EmptyAsset, GetUnifyAssetPath(Path), AssetLoadCallbacks, Async);
	}

	void ReimportAsset(std::shared_ptr<Asset>& TargetAsset, std::optional<Asset::Callbacks>& AssetLoadCallbacks = Asset::s_DefaultNullCallbacks, bool Async = true)
	{
		assert(TargetAsset);

		ReimportAssetImpl(TargetAsset, GetUnifyAssetPath(TargetAsset->GetPath()), AssetLoadCallbacks, Async);
	}
private:
	inline static std::filesystem::path GetUnifyAssetPath(const std::filesystem::path& Path, bool Lowercase = false)
	{
		auto UnifyPath = std::filesystem::path(std::move(std::filesystem::path(Path).make_preferred()));
		return Lowercase ? std::filesystem::path(StringUtils::Lowercase(UnifyPath.string())) : UnifyPath;
	}

	void CreateAssetImporters();

	std::shared_ptr<Asset> ReimportAssetImpl(
		std::shared_ptr<Asset>& TargetAsset,
		const std::filesystem::path& AssetPath, 
		std::optional<Asset::Callbacks>& AssetLoadCallbacks, 
		bool Async);

	std::unordered_map<std::filesystem::path, std::shared_ptr<struct AssetImportTask>> m_AssetLoadTasks;
	std::vector<std::unique_ptr<IAssetImporter>> m_AssetImporters;
};

