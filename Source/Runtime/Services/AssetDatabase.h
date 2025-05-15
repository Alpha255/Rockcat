#pragma once

#include "Core/StringUtils.h"
#include "Core/Module.h"
#include "Asset/Asset.h"

class AssetDatabase : public IService<AssetDatabase>
{
public:
	void Initialize() override final;
	void Finalize() override final;

	template<class TAsset>
	inline std::shared_ptr<TAsset> GetOrReimportAsset(const std::filesystem::path& Path, std::optional<Asset::AssetLoadCallbacks>& Callbacks = Asset::s_DefaultLoadCallbacks, bool Async = true)
	{
		auto UnifyPath = GetUnifyAssetPath(Path);
		return Cast<TAsset>(ReimportAssetImpl(UnifyPath, Callbacks, Async));
	}

	inline void ReimportAsset(Asset& InAsset, std::optional<Asset::AssetLoadCallbacks>& Callbacks = Asset::s_DefaultLoadCallbacks, bool Async = true)
	{
		auto UnifyPath = GetUnifyAssetPath(InAsset.GetPath());
		ReimportAssetImpl(UnifyPath, Callbacks, Async);
	}
private:
	inline static std::filesystem::path GetUnifyAssetPath(const std::filesystem::path& Path, bool Lowercase = false)
	{
		auto UnifyPath = std::filesystem::path(std::move(std::filesystem::path(Path).make_preferred()));
		return Lowercase ? std::filesystem::path(StringUtils::Lowercase(UnifyPath.string())) : UnifyPath;
	}

	void CreateAssetImporters();

	std::shared_ptr<Asset> ReimportAssetImpl(const std::filesystem::path& Path, std::optional<Asset::AssetLoadCallbacks>& Callbacks, bool Async);

	std::unordered_map<std::filesystem::path, std::shared_ptr<class AssetImportTask>> m_AssetLoadTasks;
	std::vector<std::unique_ptr<IAssetImporter>> m_AssetImporters;
};

