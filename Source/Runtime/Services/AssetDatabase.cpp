#include "Services/AssetDatabase.h"
#include "Services/SpdLogService.h"
#include "Asset/AssetLoaders/TextureLoader.h"
#include "Asset/AssetLoaders/AssimpSceneLoader.h"
#include "Async/Task.h"

void AssetDatabase::Initialize()
{
	CreateAssetLoaders();
}

void AssetDatabase::CreateAssetLoaders()
{
	m_AssetLoaders.emplace_back(std::make_unique<TextureLoader>());
	m_AssetLoaders.emplace_back(std::make_unique<AssimpSceneLoader>());
}

AssetLoader* AssetDatabase::FindAssetLoader(const std::string& Extension)
{
	for (auto& Loader : m_AssetLoaders)
	{
		if (Loader->IsSupportedFormat(Extension))
		{
			return Loader.get();
		}
	}

	return nullptr;
}

void AssetDatabase::RequestLoad(AssetLoadRequests& Requests)
{
	for (auto& Request : Requests)
	{
		ProcessAssetLoadRequest(Request);
	}
}

void AssetDatabase::LoadAssetFunc(AssetLoader& Loader, AssetLoadRequest& Request)
{
	Request.SetAssetStatus(Asset::EStatus::Loading);

	if (Loader.Load(*Request.Target))
	{
		Request.SetAssetStatus(Asset::EStatus::Ready);
	}
	else
	{
		Request.SetAssetStatus(Asset::EStatus::LoadFailed);
	}
}

void AssetDatabase::ProcessAssetLoadRequest(AssetLoadRequest& Request)
{
	std::filesystem::path UnifiedPath = GetUnifiedAssetPath(Request.Path);
	if (!std::filesystem::exists(UnifiedPath))
	{
		LOG_ERROR_CAT(LogAsset, "The target asset \"{}\" is not exists.", Request.Path);
		return;
	}

	std::string Extension = UnifiedPath.extension().string();
	if (auto Loader = FindAssetLoader(Extension))
	{
		bool NeedRestartTask = false;
		std::lock_guard Locker(m_Lock);
		auto It = m_AssetLoadTasks.find(UnifiedPath);
		if (It == m_AssetLoadTasks.end())
		{
			auto Target = Loader->CreateAsset(UnifiedPath);
			auto Task = std::make_shared<TFTask>(String::Format("LoadAsset:%s", UnifiedPath.filename().string().c_str()),
				[Loader, &Request, this]()
				{
					LoadAssetFunc(*Loader, Request);
				});

			It = m_AssetLoadTasks.emplace(std::move(UnifiedPath), AssetLoadTask{ std::move(Task), std::move(Target) }).first;
		}
		else
		{
			if (Request.ForceReload)
			{
				It->second.Target = Loader->CreateAsset(UnifiedPath);
				NeedRestartTask = true;
			}
			else
			{
				Request.Target = It->second.Target;
				return;
			}
		}

		if (Request.Async)
		{
			NeedRestartTask ? It->second.Task->Restart() : It->second.Task->Trigger();
		}
		else
		{
			LoadAssetFunc(*Loader, Request);
		}
	}
	else
	{
		LOG_ERROR_CAT(LogAsset, "The target asset \"{}\" is not supported yet.", Request.Path);
	}
}

bool AssetDatabase::Unload(const std::filesystem::path& Path)
{
	//auto It = m_AssetLoadTasks.find(Path);
	//if (It != m_AssetLoadTasks.end())
	//{
	//	It->second->DoUnload(Async);
	//	return true;
	//}

	return false;
}

bool AssetDatabase::CancelLoad(const std::filesystem::path& Path)
{
	//auto It = m_AssetLoadTasks.find(Path);
	//if (It != m_AssetLoadTasks.end())
	//{
	//	return It->second->Cancel();
	//}

	return false;
}

void AssetDatabase::Finalize()
{
	for (auto& It : m_AssetLoadTasks)
	{
		auto& Task = It.second.Task;
		if (!Task->IsCompleted())
		{
			Task->Wait();
		}
	}

	m_AssetLoadTasks.clear();
	m_AssetLoaders.clear();
}