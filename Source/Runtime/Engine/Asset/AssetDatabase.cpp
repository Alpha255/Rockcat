#include "Engine/Asset/AssetDatabase.h"
#include "Engine/Services/SpdLogService.h"
#include "Engine/Services/TaskFlowService.h"
#include "Engine/Asset/Importers/DDSImageImporter.h"
#include "Engine/Asset/Importers/StbImageImporter.h"
#include "Engine/Asset/Importers/ShaderAssetImporter.h"
#include "Engine/Asset/Importers/AssimpSceneImporter.h"
#include "Engine/Async/Task.h"

class AssetImportTask : public Task
{
public:
	AssetImportTask(
		std::shared_ptr<Asset>& TargetAsset,
		const std::filesystem::path& AssetPath, 
		IAssetImporter& AssetImporter, 
		const AssetType* Type,
		std::optional<Asset::Callbacks>& AssetLoadCallbacks)
		: Task(std::move(StringUtils::Format("ImportAsset: %s", AssetPath.generic_string().c_str())), ETaskType::General)
		, m_AssetImporter(AssetImporter)
		, m_Asset(TargetAsset ? TargetAsset : AssetImporter.CreateAsset(AssetPath))
		, m_AssetType(Type)
	{
		m_Asset->SetCallbacks(AssetLoadCallbacks);
	}

	std::shared_ptr<Asset>& GetAsset() { return m_Asset; }

	void Execute() override final
	{
		m_Asset->OnPreLoad();

		m_Asset->SetStatus(Asset::EAssetStatus::Loading);

		m_AssetImporter.LoadAssetData(m_Asset, m_AssetType->ContentsType);

		if (m_AssetImporter.Reimport(*m_Asset))
		{
			m_Asset->SetStatus(Asset::EAssetStatus::Ready);
			m_Asset->OnReady();
		}
		else
		{
			m_Asset->SetStatus(Asset::EAssetStatus::Error);
			m_Asset->OnLoadFailed();
		}

		m_Asset->FreeRawData();
	}
private:
	IAssetImporter& m_AssetImporter;
	std::shared_ptr<Asset> m_Asset;
	const AssetType* m_AssetType;
};

AssetDatabase::AssetDatabase()
	: m_EnableAsyncImport(false)
{ 
	CreateAssetImporters();
}

void AssetDatabase::CreateAssetImporters()
{
	m_AssetImporters.emplace_back(std::make_unique<DDSImageImporter>());
	m_AssetImporters.emplace_back(std::make_unique<StbImageImporter>());
	m_AssetImporters.emplace_back(std::make_unique<ShaderAssetImporter>());
	m_AssetImporters.emplace_back(std::make_unique<AssimpSceneImporter>());
}

std::shared_ptr<Asset> AssetDatabase::ReimportAssetImpl(
	std::shared_ptr<Asset>& TargetAsset,
	const std::filesystem::path& AssetPath, 
	std::optional<Asset::Callbacks>& AssetLoadCallbacks, 
	bool Async)
{
	if (std::filesystem::exists(AssetPath))
	{
		auto AssetExt = AssetPath.extension();

		for (auto& AssetImporter : m_AssetImporters)
		{
			if (auto AssetType = AssetImporter->FindValidAssetType(AssetExt))
			{
				AssetImportTask NewTask(TargetAsset, AssetPath, *AssetImporter, AssetType, AssetLoadCallbacks);
				auto NewAsset = NewTask.GetAsset();

				m_Assets.insert(std::make_pair(AssetPath, NewAsset));

				if (Async && m_EnableAsyncImport)
				{
					TF_DispatchTask(NewTask, EThread::WorkerThread);
				}
				else
				{
					NewTask.Execute();
				}

				return NewAsset;
			}
		}

		LOG_ERROR("AssetDatabase::Unknown asset type: AssetPath:\"{}\"", AssetPath.generic_string());
		return nullptr;
	}

	LOG_ERROR("AssetDatabase:: Asset \"{}\" do not exists.", AssetPath.generic_string());
	return nullptr;
}

void AssetDatabase::OnShutdown()
{
	m_Assets.clear();
	m_AssetImporters.clear();
}