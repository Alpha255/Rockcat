#include "Engine/Asset/AssetDatabase.h"
#include "Engine/Services/SpdLogService.h"
#include "Engine/Services/TaskFlowService.h"
#include "Engine/Asset/Importers/DDSTextureImporter.h"
#include "Engine/Asset/Importers/StbTextureImporter.h"
#include "Engine/Asset/Importers/AssimpSceneImporter.h"
#include "Engine/Async/Task.h"

AssetImportTask::AssetImportTask(
	std::shared_ptr<::Asset>& InAsset,
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

void AssetImportTask::Execute()
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

AssetDatabase::AssetDatabase()
	: m_EnableAsyncImport(false)
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

	auto AssetTaskIt = m_AssetLoadTasks.find(AssetPath);
	if (AssetTaskIt != m_AssetLoadTasks.end())
	{
		Task = AssetTaskIt->second.get();
		Task->Asset->SetCallbacks(AssetLoadCallbacks);
	}
	else
	{
		for (auto& AssetImporter : m_AssetImporters)
		{
			if (auto AssetType = AssetImporter->FindValidAssetType(AssetPath))
			{
				Task = m_AssetLoadTasks.insert(std::make_pair(AssetPath,
					std::make_shared<AssetImportTask>(TargetAsset,
						AssetPath,
						*AssetImporter,
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

	if (Async && m_EnableAsyncImport)
	{
		tf::DispatchTask(*Task);
	}
	else
	{
		Task->Execute();
	}

	return Task->Asset;
}

void AssetDatabase::OnShutdown()
{
	m_AssetLoadTasks.clear();
	m_AssetImporters.clear();
}