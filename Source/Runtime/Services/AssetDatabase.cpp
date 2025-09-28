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
	AssetLoadTask(const std::filesystem::path& Path, AssetLoader& Loader)
		: Task(StringUtils::Format("AssetLoadTask|%s", Path.string().c_str()))
		, m_Loader(Loader)
	{
	}

	std::shared_ptr<Asset> GetAsset() const { return m_Asset; }
protected:
	void ExecuteImpl() override final
	{
		m_Asset->OnPreLoad();

		if (m_Importer.Reimport(*m_Asset, m_Type))
		{
			m_Asset->OnReady();
		}
		else
		{
			m_Asset->OnLoadFailed();
		}
	}
private:
	AssetLoader& m_Loader;
	std::shared_ptr<Asset> m_Asset;
};

void AssetDatabase::Initialize()
{
	REGISTER_LOG_CATEGORY(LogAsset);

	CreateAssetLoaders();
}

void AssetDatabase::CreateAssetLoaders()
{
	m_AssetLoaders.emplace_back(std::make_unique<DDSTextureLoader>());
	m_AssetLoaders.emplace_back(std::make_unique<StbTextureLoader>());
	m_AssetLoaders.emplace_back(std::make_unique<AssimpSceneLoader>());
}

std::shared_ptr<Asset> AssetDatabase::LoadAssetImpl(const std::filesystem::path& Path, bool Async)
{
	if (!std::filesystem::exists(Path))
	{
		LOG_CAT_ERROR(LogAsset, "Asset \"{}\" do not exists.", Path.string());
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
			if (auto AssetType = Importer->FindValidAssetType(Path))
			{
				Task = m_AssetLoadTasks.insert(std::make_pair(Path,
					std::make_shared<AssetImportTask>(Path, *Importer, *AssetType, Callbacks))).first->second.get();
			}
		}
	}

	if (!Task)
	{
		LOG_ERROR("AssetDatabase::The asset type of \"{}\" is not supported!", Path.string());
		return nullptr;
	}
	else
	{
		if (Async)
		{
			Task->Dispatch();
		}
		else
		{
			Task->Execute();
		}

		return Task->GetAsset();
	}
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