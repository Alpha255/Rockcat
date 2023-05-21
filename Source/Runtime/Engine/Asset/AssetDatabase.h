#pragma once

#include "Runtime/Engine/Asset/Asset.h"

class AssetDatabase
{
public:
	std::shared_ptr<Asset> FindAsset(const char8_t* AssetPath) const;
protected:
private:
};

