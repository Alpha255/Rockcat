#include "Services/AssetDatabase.h"
#include "Services/SpdLogService.h"
#include "Services/TaskFlowService.h"
#include "Asset/Importers/DDSTextureImporter.hpp"
#include "Asset/Importers/StbTextureImporter.hpp"
#include "Asset/Importers/AssimpSceneImporter.hpp"
#include "Async/Task.h"

class AssetImportTask : public Task
{
public:
	AssetImportTask(const std::filesystem::path& Path, IAssetImporter& Importer, const AssetType& Type, std::optional<Asset::AssetLoadCallbacks>& Callbacks)
		: Task(StringUtils::Format("AssetImportTask|%s", Path.string().c_str()))
		, m_Importer(Importer)
		, m_Type(Type)
	{
		m_Asset = Importer.CreateAsset(Path);
		m_Asset->SetLoadCallbacks(Callbacks);
	}

	void ResetLoadCallbacks(std::optional<Asset::AssetLoadCallbacks>& Callbacks)
	{
		assert(m_Asset);
		m_Asset->SetLoadCallbacks(Callbacks);
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
	IAssetImporter& m_Importer;
	std::shared_ptr<Asset> m_Asset;
	const AssetType& m_Type;
};

void AssetDatabase::Initialize()
{ 
	CreateAssetImporters();
}

void AssetDatabase::CreateAssetImporters()
{
	REGISTER_LOG_CATEGORY(LogImageImporter);
	REGISTER_LOG_CATEGORY(LogAssimpImporter);

	m_AssetImporters.emplace_back(std::make_unique<DDSImageImporter>());
	m_AssetImporters.emplace_back(std::make_unique<StbImageImporter>());
	m_AssetImporters.emplace_back(std::make_unique<AssimpSceneImporter>());
}

std::shared_ptr<Asset> AssetDatabase::ReimportAssetImpl(const std::filesystem::path& Path, std::optional<Asset::AssetLoadCallbacks>& Callbacks, bool Async)
{
	if (!std::filesystem::exists(Path))
	{
		LOG_ERROR("AssetDatabase:: Asset \"{}\" do not exists.", Path.string());
		return nullptr;
	}

	AssetImportTask* Task = nullptr;

	auto It = m_AssetLoadTasks.find(Path);
	if (It != m_AssetLoadTasks.end())
	{
		Task = It->second.get();
		assert(Task);
		Task->ResetLoadCallbacks(Callbacks);
	}
	else
	{
		for (auto& Importer : m_AssetImporters)
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
	m_AssetImporters.clear();
}