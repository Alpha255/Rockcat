#pragma once

#include "Runtime/Engine/Asset/Asset.h"
#include "Runtime/Core/StringUtils.h"

DECLARE_OBJECT_ID(Asset, uint32_t)

class IAssetLoader
{
};

class AssetDatabase
{
public:
	AssetDatabase();
	~AssetDatabase();

	template<class TAsset>
	const TAsset* FindAsset(const char8_t* AssetPath)
	{
		assert(AssetPath);

		auto CaseInsensitiveAssetPath = StringUtils::Lowercase(AssetPath);
		auto AssetIt = m_Assets.find(CaseInsensitiveAssetPath);
		if (AssetIt != m_Assets.end())
		{
			return Cast<TAsset>(*AssetIt);
		}
		else
		{
			return Cast<TAsset>(LoadAsset(CaseInsensitiveAssetPath));
		}
	}
private:
	void RegisterAssetType(const char8_t* AssetTypeName, std::vector<std::string_view>& Extensions, IAssetLoader* Loader);

	const Asset* LoadAsset(const std::string& AssetPath);

	std::unordered_map<std::string, std::unique_ptr<Asset>> m_Assets;

	std::unordered_map<std::string, AssetID> m_AssetNameIDs;

	std::vector<AssetType> m_SupportedAssetTypes;
};

