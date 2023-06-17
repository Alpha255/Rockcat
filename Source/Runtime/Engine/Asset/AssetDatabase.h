#pragma once

#include "Runtime/Engine/Asset/Asset.h"
#include "Runtime/Core/StringUtils.h"

DECLARE_OBJECT_ID(Asset, uint32_t)

class AssetDatabase
{
public:
	AssetDatabase();

	~AssetDatabase();

	template<class TAsset>
	const TAsset* FindAsset(const char8_t* AssetPath)
	{
		assert(AssetPath);

		auto CaseInsensitiveAssetPath = StringUtils::Replace(StringUtils::Lowercase(AssetPath), "/", "\\");
		auto AssetIt = m_Assets.find(CaseInsensitiveAssetPath);
		if (AssetIt != m_Assets.end())
		{
			return Cast<TAsset>(*AssetIt);
		}
		else
		{
			return Cast<TAsset>(ReimportAsset(CaseInsensitiveAssetPath));
		}
	}
private:
	void CreateAssetImporters();

	const Asset* ReimportAsset(const std::string& AssetPath);

	std::unordered_map<std::string, std::shared_ptr<Asset>> m_Assets;
	std::vector<std::unique_ptr<IAssetImporter>> m_AssetImporters;
	bool8_t m_AsyncLoadAssets;
};

