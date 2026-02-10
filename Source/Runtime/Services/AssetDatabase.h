#pragma once

#include "Core/Module.h"
#include "Core/StringUtils.h"
#include "Asset/Asset.h"

class AssetDatabase : public IService<AssetDatabase>
{
public:
	void Initialize() override final;
	void Finalize() override final;

	template<class T>
	inline std::shared_ptr<T> Load(const std::filesystem::path& Path, bool ForceReload = false, bool Async = true)
	{
		return Cast<T>(LoadAssetImpl(GetUnifyAssetPath(Path), ForceReload, Async));
	}

	bool Unload(const std::filesystem::path& Path);

	bool CancelLoad(const std::filesystem::path& Path);
private:
	inline static std::filesystem::path GetUnifyAssetPath(const std::filesystem::path& Path, bool Lowercase = false)
	{
		auto UnifyPath = std::filesystem::path(std::move(std::filesystem::path(Path).make_preferred()));
		return Lowercase ? std::filesystem::path(String::Lowercase(UnifyPath.string())) : UnifyPath;
	}

	void CreateAssetLoaders();

	std::shared_ptr<Asset> LoadAssetImpl(const std::filesystem::path& Path, bool ForceReload, bool Async);

	std::shared_ptr<Asset> ProcessAssetLoadRequest(const AssetLoadRequest& Request);

	std::unordered_map<std::filesystem::path, std::shared_ptr<class AssetLoadTask>> m_AssetLoadTasks;
	std::unordered_map<std::filesystem::path, AssetLoadRequest> m_AssetLoadRequests;
	std::vector<std::unique_ptr<AssetLoader>> m_AssetLoaders;
};

