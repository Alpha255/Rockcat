#pragma once

#include "Core/Singleton.h"
#include "Core/String.h"
#include "Asset/Asset.h"

class AssetDatabase : public Singleton<AssetDatabase>
{
public:
	void Initialize();
	void Finalize();

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

	inline static std::filesystem::path GetUnifiedAssetPath(const string& Path, bool Lowercase = false)
	{
		std::string UnifiedPath = Lowercase ? Path.lowercase() : Path;
		return std::filesystem::path(UnifiedPath).make_preferred();
	}

	AssetLoader* FindAssetLoader(const string& Extension);

	void CreateAssetLoaders();

	void ProcessAssetLoadRequest(AssetLoadRequest& Request);

	static void LoadAssetFunc(AssetLoader& Loader, AssetLoadRequest& Request);

	std::unordered_map<std::filesystem::path, AssetLoadTask> m_AssetLoadTasks;
	std::vector<std::unique_ptr<AssetLoader>> m_AssetLoaders;

	std::mutex m_Lock;
};

