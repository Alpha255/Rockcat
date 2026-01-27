#include "Services/AssetDatabase.h"
#include "Services/SpdLogService.h"
#include "Services/TaskFlowService.h"
#include "Asset/AssetLoaders/DDSTextureLoader.h"
#include "Asset/AssetLoaders/StbTextureLoader.h"
#include "Asset/AssetLoaders/AssimpSceneLoader.h"
#include "Async/Task.h"

class AssetLoadTask : public Task
{
public:
	AssetLoadTask(const std::filesystem::path& Path, const AssetType& Type, AssetLoader& Loader)
		: Task(String::Format("AssetLoadTask|%s", Path.filename().c_str()))
		, m_Asset(std::move(Loader.CreateAsset(Path)))
		, m_Type(Type)
		, m_Loader(Loader)
	{
	}

	std::shared_ptr<Asset> GetAsset() const { return m_Asset; }

	inline void DoLoad(bool Async)
	{
		DoTask(EOperation::Load, Async);
	}

	inline void DoUnload(bool Async)
	{
		DoTask(EOperation::Unload, Async);
	}
protected:
	enum class EOperation : uint8_t
	{
		Load,
		Unload
	};

	void DoTask(EOperation Operation, bool Async)
	{
		m_Operation = Operation;

		if (Async)
		{
			Dispatch();
		}
		else
		{
			Execute();
		}
	}

	void ExecuteImpl() override final
	{
		if (m_Operation == EOperation::Unload)
		{
			if (m_Asset->IsReady())
			{
				m_Asset->OnUnload();
			}
		}
		else if (m_Operation == EOperation::Load)
		{
			m_Asset->OnPreLoad();

			if (m_Loader.Load(*m_Asset, m_Type))
			{
				m_Asset->OnReady();
				m_Asset->OnPostLoad();
			}
			else
			{
				m_Asset->OnLoadFailed();
			}
		}
	}
private:
	std::shared_ptr<Asset> m_Asset;
	const AssetType& m_Type;
	AssetLoader& m_Loader;

	EOperation m_Operation = EOperation::Load;
};

void AssetDatabase::Initialize()
{
	CreateAssetLoaders();
}

void AssetDatabase::CreateAssetLoaders()
{
	m_AssetLoaders.emplace_back(std::make_unique<DDSTextureLoader>());
	m_AssetLoaders.emplace_back(std::make_unique<StbTextureLoader>());
	m_AssetLoaders.emplace_back(std::make_unique<AssimpSceneLoader>());
}

std::shared_ptr<Asset> AssetDatabase::LoadAssetImpl(const std::filesystem::path& Path, bool ForceReload, bool Async)
{
	if (!std::filesystem::exists(Path))
	{
		LOG_ERROR_CAT(LogAsset, "Asset \"{}\" do not exists.", Path.string());
		return nullptr;
	}

	AssetLoadTask* LoadTask = nullptr;

	auto It = m_AssetLoadTasks.find(Path);
	if (It != m_AssetLoadTasks.end())
	{
		LoadTask = It->second.get();
		assert(LoadTask);
	}
	else
	{
		for (auto& Loader : m_AssetLoaders)
		{
			if (auto AssetType = Loader->TryFindAssetTypeByPath(Path))
			{
				LoadTask = m_AssetLoadTasks.insert(std::make_pair(Path,
					std::make_shared<AssetLoadTask>(Path, *AssetType, *Loader))).first->second.get();
				break;
			}
		}
	}

	if (!LoadTask)
	{
		LOG_ERROR_CAT(LogAsset, "The asset type of \"{}\" is not supported!", Path.string());
		return nullptr;
	}
	else
	{
		if (!LoadTask->GetAsset()->IsOnLoading())
		{
			const bool NeedReload = ForceReload || !LoadTask->GetAsset()->IsReady();
			if (NeedReload)
			{
				LoadTask->DoLoad(Async);
			}
		}

		return LoadTask->GetAsset();
	}
}

std::shared_ptr<Asset> AssetDatabase::ProcessAssetLoadRequest(const AssetLoadRequest& Request)
{
	if (Request.Async)
	{

	}
	else
	{

	}

	return std::shared_ptr<Asset>();
}

bool AssetDatabase::Unload(const std::filesystem::path& Path, bool Async)
{
	auto It = m_AssetLoadTasks.find(Path);
	if (It != m_AssetLoadTasks.end())
	{
		It->second->DoUnload(Async);
		return true;
	}

	return false;
}

bool AssetDatabase::CancelLoad(const std::filesystem::path& Path)
{
	auto It = m_AssetLoadTasks.find(Path);
	if (It != m_AssetLoadTasks.end())
	{
		return It->second->Cancel();
	}

	return false;
}

void AssetDatabase::Finalize()
{
	for (auto& It : m_AssetLoadTasks)
	{
		auto& Task = It.second;
		if (!Task->IsCompleted())
		{
			Task->Wait();
		}
	}

	m_AssetLoadTasks.clear();
	m_AssetLoaders.clear();
}