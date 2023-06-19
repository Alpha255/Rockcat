#include "Runtime/Engine/Asset/AssetDatabase.h"
#include "Runtime/Engine/Asset/Importers/AssimpSceneImporter.h"
#include "Runtime/Engine/Asset/Importers/DDSImageImporter.h"
#include "Runtime/Engine/Asset/Importers/StbImageImporter.h"
#include "Runtime/Engine/Asset/Importers/ShaderAssetImporter.h"
#include "Runtime/Engine/Async/Task.h"

class AssetImportTask : public Task
{
public:
	AssetImportTask(const std::filesystem::path& AssetPath, IAssetImporter& AssetImporter)
		: Task(std::move(std::string("ImportAsset:") + AssetPath.u8string()), ETaskType::General)
		, m_AssetImporter(AssetImporter)
		, m_Asset(std::move(AssetImporter.CreateAsset(AssetPath)))
	{
	}

	std::shared_ptr<Asset> GetAsset() const { return m_Asset; }

	void DoTask() override final
	{
		m_Asset->SetStatus(Asset::EAssetStatus::Loading);

		auto Succeed = m_AssetImporter.Reimport(*m_Asset);

		m_Asset->SetStatus(Succeed ? Asset::EAssetStatus::Ready : Asset::EAssetStatus::Error);
	}
private:
	IAssetImporter& m_AssetImporter;
	std::shared_ptr<Asset> m_Asset;
};

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

const Asset* AssetDatabase::ReimportAsset(const std::filesystem::path& AssetPath)
{
	assert(std::filesystem::exists(AssetPath));

	auto AssetExt = AssetPath.extension().u8string();

	for each (auto& AssetImporter in m_AssetImporters)
	{
		if (AssetImporter->IsValidAssetExtension(AssetExt.c_str()))
		{
			AssetImportTask NewTask(AssetPath, *AssetImporter);
			auto NewAsset = NewTask.GetAsset();

			if (m_AsyncLoadAssets)
			{
				TF_DispatchTask(NewTask);
			}
			else
			{
				NewTask.DoTask();
			}

			m_Assets.insert(std::make_pair(AssetPath, NewAsset));
			return NewAsset.get();
		}
	}

	LOG_ERROR("AssetDatabase::Unknown asset type: AssetPath:\"{}\"", AssetPath.u8string());
	return nullptr;
}

AssetDatabase::~AssetDatabase()
{
}
