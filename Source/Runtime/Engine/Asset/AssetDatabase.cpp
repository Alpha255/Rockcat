#if 0
AssetDatabase::AssetDatabase()
{
	m_AsyncExecutor = std::make_shared<tf::Executor>();
}
#endif

#include "Runtime/Engine/Asset/AssetDatabase.h"
#include "Runtime/Engine/Asset/Importers/AssimpSceneImporter.h"
#include "Runtime/Engine/Asset/Importers/DDSImageImporter.h"
#include "Runtime/Engine/Asset/Importers/StbImageImporter.h"
#include "Runtime/Engine/Asset/Importers/ShaderAssetImporter.h"
#include "Runtime/Engine/Async/Task.h"

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
			auto NewAsset = AssetImporter->CreateAsset(AssetPath);

			if (m_AsyncLoadAssets)
			{
				NewAsset->SetStatus(Asset::EAssetStatus::Queued);
			}
			else
			{
				NewAsset->SetStatus(Asset::EAssetStatus::Loading);
				AssetImporter->Reimport(*NewAsset);
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
