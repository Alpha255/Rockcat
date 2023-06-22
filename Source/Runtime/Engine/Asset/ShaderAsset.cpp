#include "Runtime/Engine/Asset/ShaderAsset.h"
#include "Runtime/Engine/Asset/ShaderCompiler.h"
#include "Runtime/Engine/Engine.h"
#include "Runtime/Engine/Async/Task.h"

std::string ShaderCache::ConvertToShaderCachePath(const char8_t* ShaderName)
{
	return StringUtils::Format(
		"%s%s.%s%s",
		ASSET_PATH_SHADERCACHE,
		ShaderName,
		"Vulkan",
		Asset::GetPrefabricateAssetExtension(Asset::EPrefabricateAssetType::ShaderCacheAsset));
}

class ShaderCompileTask : public Task
{
public:
	ShaderCompileTask(const char8_t* ShaderName)
		: Task(ShaderName, ETaskType::ShaderCompile, EPriority::High)
	{
	}
};