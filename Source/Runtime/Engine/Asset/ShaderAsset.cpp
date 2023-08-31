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

class GlobalShaderCompileConfigurations : public SerializableAsset<GlobalShaderCompileConfigurations>
{
public:
	using BaseClass::BaseClass;
};

class ShaderCompileTask : public Task
{
public:
	ShaderCompileTask(const char8_t* ShaderName)
		: Task(std::move(StringUtils::Format("Compile shader: %s ...", ShaderName)), ETaskType::ShaderCompile, EPriority::High)
	{
	}

	void DoTask() override final
	{
	}
};

void ShaderAsset::Compile(bool8_t Force)
{
	bool8_t NeedRecompile = Force || IsDirty();
	
	if (!IsLoading() && NeedRecompile)
	{
		SetStatus(Asset::EAssetStatus::Loading);
	}
}
