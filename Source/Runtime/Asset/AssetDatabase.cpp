#include "Asset/AssetDatabase.h"
#include "Services/SpdLogService.h"
#include "Services/TaskFlowService.h"
#include "Asset/Importers/DDSTextureImporter.hpp"
#include "Asset/Importers/StbTextureImporter.hpp"
#include "Asset/Importers/AssimpSceneImporter.hpp"
#include "Async/Task.h"

struct AssetImportTask : public Task
{
	AssetImportTask(std::shared_ptr<::Asset>& InAsset,
		const std::filesystem::path& InPath,
		IAssetImporter& InImporter,
		const AssetType& InType,
		std::optional<Asset::Callbacks>& InCallbacks)
		: Task(std::move(StringUtils::Format("AssetImportTask|%s", InPath.string().c_str())))
		, Importer(InImporter)
		, Asset(InAsset ? InAsset : InImporter.CreateAsset(InPath))
		, Type(InType)
	{
		Asset->SetCallbacks(InCallbacks);
	}

	IAssetImporter& Importer;
	std::shared_ptr<Asset> Asset;
	const AssetType& Type;

protected:
	void ExecuteImpl() override final
	{
		Asset->OnPreLoad();

		Asset->SetStatus(Asset::EStatus::Loading);

		Importer.LoadAssetData(Asset, Type.ContentsType);

		if (Importer.Reimport(*Asset, Type))
		{
			Asset->SetStatus(Asset::EStatus::Ready);
			Asset->OnReady();
		}
		else
		{
			Asset->SetStatus(Asset::EStatus::Error);
			Asset->OnLoadFailed();
		}

		Asset->FreeRawData();
	}
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

std::shared_ptr<Asset> AssetDatabase::ReimportAssetImpl(
	std::shared_ptr<Asset>& TargetAsset,
	const std::filesystem::path& AssetPath, 
	std::optional<Asset::Callbacks>& AssetLoadCallbacks, 
	bool Async)
{
	if (!std::filesystem::exists(AssetPath))
	{
		LOG_ERROR("AssetDatabase:: Asset \"{}\" do not exists.", AssetPath.string());
		return nullptr;
	}

	AssetImportTask* Task = nullptr;

	auto It = m_AssetLoadTasks.find(AssetPath);
	if (It != m_AssetLoadTasks.end())
	{
		Task = It->second.get();
		Task->Asset->SetCallbacks(AssetLoadCallbacks);
	}
	else
	{
		for (auto& Importer : m_AssetImporters)
		{
			if (auto AssetType = Importer->FindValidAssetType(AssetPath))
			{
				Task = m_AssetLoadTasks.insert(std::make_pair(AssetPath,
					std::make_shared<AssetImportTask>(TargetAsset,
						AssetPath,
						*Importer,
						*AssetType,
						AssetLoadCallbacks))).first->second.get();
			}
		}
	}

	if (!Task)
	{
		LOG_ERROR("AssetDatabase::The asset type of \"{}\" is not supported!", AssetPath.string());
		return nullptr;
	}

	if (Async)
	{
		Task->Dispatch();
	}
	else
	{
		Task->Execute();
	}

	return Task->Asset;
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