#include "Runtime/Engine/Asset/ShaderAsset.h"
#include "Runtime/Engine/Asset/ShaderCompiler.h"
#include "Runtime/Engine/Async/Task.h"

std::string ShaderCache::GetShaderCachePath(const char8_t* ShaderName)
{
	return StringUtils::Format(
		"%s%s.%s%s",
		ASSET_PATH_SHADERCACHE,
		ShaderName,
		"Vulkan",
		Asset::GetPrefabricateAssetExtension(Asset::EPrefabAssetType::ShaderCache));
}

class ShaderCompileTask : public Task
{
public:
	ShaderCompileTask(const char8_t* ShaderName)
		: Task(std::move(std::string("Compile shader:") + ShaderName), ETaskType::ShaderCompile, EPriority::High)
	{
	}

	void DoTask() override final
	{
	}
private:
	ShaderDefines m_Definitions;
};