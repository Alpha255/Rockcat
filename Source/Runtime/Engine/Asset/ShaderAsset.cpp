#include "Runtime/Engine/Asset/ShaderAsset.h"
#include "Runtime/Engine/Asset/ShaderCompiler.h"
#include "Runtime/Engine/Engine.h"

std::string ShaderCache::ConvertToShaderCachePath(const char8_t* ShaderName)
{
	return StringUtils::Format(
		"%s%s.%s%s",
		ASSET_PATH_SHADERCACHE,
		ShaderName,
		"Vulkan",
		GetAssetExtension());
}