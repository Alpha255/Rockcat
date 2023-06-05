#pragma once

#include "Runtime/Engine/Asset/Asset.h"
#include "Runtime/Core/StringUtils.h"

class IAssetLoader
{
};

class AssetDatabase
{
public:
	template<class TAsset>
	const TAsset* FindAsset(const char8_t* AssetPath) const
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
			return LoadAsset(CaseInsensitiveAssetPath);
		}
	}
private:
	const Asset* LoadAsset(const std::string& AssetPath);

	std::unordered_map<std::string, std::unique_ptr<Asset>> m_Assets;
};

