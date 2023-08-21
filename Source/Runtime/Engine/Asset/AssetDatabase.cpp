#include "Runtime/Engine/Asset/AssetDatabase.h"
#include "Runtime/Engine/Services/SpdLogService.h"
#include "Runtime/Engine/Services/TaskFlowService.h"
#include "Runtime/Engine/Asset/Importers/AssimpSceneImporter.h"
#include "Runtime/Engine/Asset/Importers/DDSImageImporter.h"
#include "Runtime/Engine/Asset/Importers/StbImageImporter.h"
#include "Runtime/Engine/Asset/Importers/ShaderAssetImporter.h"
#include "Runtime/Engine/Async/Task.h"

class AssetImportTask : public Task
{
public:
	AssetImportTask(const std::filesystem::path& AssetPath, IAssetImporter& AssetImporter, const AssetType* Type)
		: Task(std::move(std::string("ImportAsset:") + AssetPath.generic_string()), ETaskType::General)
		, m_AssetImporter(AssetImporter)
		, m_Asset(std::move(AssetImporter.CreateAsset(AssetPath)))
		, m_AssetType(Type)
	{
	}

	std::shared_ptr<Asset> GetAsset() const { return m_Asset; }

	void DoTask() override final
	{
		m_Asset->OnPreLoad();

		m_Asset->SetStatus(Asset::EAssetStatus::Loading);

		m_Asset->ReadRawData(m_AssetType->ContentsType);

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
	}
private:
	IAssetImporter& m_AssetImporter;
	std::shared_ptr<Asset> m_Asset;
	const AssetType* m_AssetType;
};

AssetDatabase& AssetDatabase::Get()
{
	static AssetDatabase s_AssetDatabase;
	return s_AssetDatabase;
}

AssetDatabase::AssetDatabase()
	: m_AsyncLoadAssets(false)
{ 
	CreateAssetImporters();
}

void AssetDatabase::CreateAssetImporters()
{
	m_AssetImporters.emplace_back(std::make_unique<AssimpSceneImporter>());
	m_AssetImporters.emplace_back(std::make_unique<DDSImageImporter>());
	m_AssetImporters.emplace_back(std::make_unique<StbImageImporter>());
	m_AssetImporters.emplace_back(std::make_unique<ShaderAssetImporter>());
}

Asset* AssetDatabase::ReimportAssetInternal(const std::filesystem::path& AssetPath, std::optional<Asset::Callbacks>& AssetLoadCallbacks)
{
	if (std::filesystem::exists(AssetPath))
	{
		auto AssetExt = AssetPath.extension();

		for each (auto & AssetImporter in m_AssetImporters)
		{
			if (auto AssetType = AssetImporter->FindValidAssetType(AssetExt))
			{
				AssetImportTask NewTask(AssetPath, *AssetImporter, AssetType);
				auto NewAsset = NewTask.GetAsset();
				NewAsset->SetCallbacks(AssetLoadCallbacks);

				m_Assets.insert(std::make_pair(AssetPath, NewAsset));

				if (m_AsyncLoadAssets)
				{
					TF_DispatchTask(NewTask);
				}
				else
				{
					NewTask.DoTask();
				}

				return NewAsset.get();
			}
		}

		LOG_ERROR("AssetDatabase::Unknown asset type: AssetPath:\"{}\"", AssetPath.generic_string());
		return nullptr;
	}

	LOG_ERROR("AssetDatabase:: Asset \"{}\" do not exists.", AssetPath.generic_string());
	return nullptr;
}

AssetDatabase::~AssetDatabase()
{
}
