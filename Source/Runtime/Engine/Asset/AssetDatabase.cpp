#include "Runtime/Engine/Asset/AssetDatabase.h"
#include "Runtime/Engine/Services/SpdLogService.h"
#include "Runtime/Engine/Services/TaskFlowService.h"
#include "Runtime/Engine/Asset/Importers/DDSImageImporter.h"
#include "Runtime/Engine/Asset/Importers/StbImageImporter.h"
#include "Runtime/Engine/Asset/Importers/ShaderAssetImporter.h"
#include "Runtime/Engine/Asset/Importers/AssimpSceneImporter.h"
#include "Runtime/Engine/Async/Task.h"

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

	std::shared_ptr<Asset> GetAsset() const { return m_Asset; }

	void DoTask() override final
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
	: m_SupportAsyncLoad(false)
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
	bool8_t Async)
{
	if (std::filesystem::exists(AssetPath))
	{
		auto AssetExt = AssetPath.extension();

		for each (auto & AssetImporter in m_AssetImporters)
		{
			if (auto AssetType = AssetImporter->FindValidAssetType(AssetExt))
			{
				AssetImportTask NewTask(TargetAsset, AssetPath, *AssetImporter, AssetType, AssetLoadCallbacks);
				auto NewAsset = NewTask.GetAsset();

				m_Assets.insert(std::make_pair(AssetPath, NewAsset));

				if (Async && m_SupportAsyncLoad)
				{
					TF_DispatchTask(NewTask);
				}
				else
				{
					NewTask.DoTask();
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