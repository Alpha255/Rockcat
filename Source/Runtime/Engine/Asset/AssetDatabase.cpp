#if 0
#include "Runtime/Asset/AssetDatabase.h"
#include "Runtime/Asset/AssimpImporter.h"
#include "Runtime/Asset/ImageImporter.h"
#include "Runtime/Asset/ShaderImporter.h"
#include "Colorful/IRenderer/IRenderer.h"
#include "AssetDatabase.h"

AssetDatabase::AssetDatabase()
{
	m_AsyncExecutor = std::make_shared<tf::Executor>();

	auto RHIDevice = RHI::IRenderer::Get().Device();

	m_Importers[IAsset::ECategory::Model] = std::make_shared<AssimpImporter>(RHIDevice);
	m_Importers[IAsset::ECategory::Image] = std::make_shared<ImageImporter>(RHIDevice);
	m_Importers[IAsset::ECategory::Shader] = std::make_shared<ShaderImporter>(RHIDevice);
}

AssetDatabase::~AssetDatabase()
{
	if (Material::DefaultImage)
	{
		Material::DefaultImage.reset();
	}

	m_AsyncExecutor->wait_for_all();

	for (auto& AssetMap : m_Assets)
	{
		AssetMap.second.clear();
	}
}
#endif

#include "Runtime/Engine/Asset/AssetDatabase.h"
#include "Runtime/Engine/Asset/SceneAsset.h"
#include "Runtime/Engine/Asset/ImageAsset.h"
#include "Runtime/Engine/Asset/ShaderAsset.h"
#include "Runtime/Engine/Asset/MaterialAsset.h"

#define REGISTER_ASSET_TYPE(Type, Extensions, Loader) RegisterAssetType(#Type, Extensions, Loader)

AssetDatabase::AssetDatabase()
{
	REGISTER_ASSET_TYPE(SceneAsset, { ".scene" }, nullptr);
	REGISTER_ASSET_TYPE(AssimpSceneAsset, { ".scene" }, nullptr);
}

void AssetDatabase::RegisterAssetType(const char8_t* AssetTypeName, std::vector<std::string_view>& Extensions, IAssetLoader* Loader)
{
	assert(AssetTypeName && Loader && Extensions.size() > 0u);
	m_SupportedAssetTypes.emplace_back(AssetType(AssetTypeName, Extensions, Loader));
}

const Asset* AssetDatabase::LoadAsset(const std::string& AssetPath)
{
	auto AssetExt = std::filesystem::path(AssetPath).extension().c_str();
	return nullptr;
}

AssetDatabase::~AssetDatabase()
{
}
