#if 0
#include "Runtime/Asset/AssetDatabase.h"
#include "Runtime/Asset/AssimpImporter.h"
#include "Runtime/Asset/ImageImporter.h"
#include "Runtime/Asset/ShaderImporter.h"
#include "Colorful/IRenderer/IRenderer.h"

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
