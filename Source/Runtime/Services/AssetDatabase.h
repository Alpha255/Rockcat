#pragma once

#include "Core/Module.h"
#include "Core/StringUtils.h"
#include "Asset/Asset.h"

class AssetDatabase : public IService<AssetDatabase>
{
public:
	void Initialize() override final;
	void Finalize() override final;

	inline void RequestLoad(AssetLoadRequest& Request)
	{
		AssetLoadRequests Requests{ Request };
		RequestLoad(Request);
	}

	void RequestLoad(AssetLoadRequests& Requests);

	inline bool Unload(const AssetLoadRequest& Request)
	{
		return Unload(GetUnifiedAssetPath(Request.Path));
	}

	inline bool CancelLoad(const AssetLoadRequest& Request)
	{
		return CancelLoad(GetUnifiedAssetPath(Request.Path));
	}

	bool Unload(const std::filesystem::path& Path);

	bool CancelLoad(const std::filesystem::path& Path);
private:
	struct AssetLoadTask
	{
		std::shared_ptr<class TFTask> Task;
		std::shared_ptr<Asset> Target;
	};

	inline static std::filesystem::path GetUnifiedAssetPath(const std::string_view& Path, bool Lowercase = false)
	{
		auto UnifiedPath = Lowercase ? std::filesystem::path(std::move(String::Lowercase(std::string(Path)))) : std::filesystem::path(Path);
		UnifiedPath.make_preferred();
		return UnifiedPath;
	}

	AssetLoader* FindAssetLoader(const std::string& Extension);

	void CreateAssetLoaders();

	void ProcessAssetLoadRequest(AssetLoadRequest& Request);

	static void LoadAssetFunc(AssetLoader& Loader, AssetLoadRequest& Request);

	std::unordered_map<std::filesystem::path, AssetLoadTask> m_AssetLoadTasks;
	std::vector<std::unique_ptr<AssetLoader>> m_AssetLoaders;

	std::mutex m_Lock;
};

