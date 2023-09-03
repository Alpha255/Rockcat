#include "Runtime/Engine/Asset/ShaderAsset.h"
#include "Runtime/Engine/Asset/ShaderCompiler.h"
#include "Runtime/Engine/RHI/RHIInterface.h"

class GlobalShaderCompileConfigurations : public SerializableAsset<GlobalShaderCompileConfigurations>
{
public:
	using BaseClass::BaseClass;

	static std::shared_ptr<GlobalShaderCompileConfigurations> Get()
	{
		static std::filesystem::path s_Path = (std::filesystem::path(ASSET_PATH_SHADERS) /
			"GlobalShaderCompileConfigs").replace_extension(Asset::GetPrefabricateAssetExtension(Asset::EPrefabAssetType::Config));

		return GlobalShaderCompileConfigurations::Load(s_Path);
	}

	const ShaderDefines& GetDefines(const std::filesystem::path& ShaderPath) const
	{
		static ShaderDefines EmptyDefines;
		const auto It = m_DefaultDefines.find(ShaderPath.generic_string());
		return It == m_DefaultDefines.cend() ? EmptyDefines : (*It).second;
	}

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_NVP(m_DefaultDefines)
		);
	}
private:
	std::map<std::string, ShaderDefines> m_DefaultDefines;
};

void ShaderAsset::GetDefaultDefines()
{
	Merge(GlobalShaderCompileConfigurations::Get()->GetDefines(GetPath()));
}

void ShaderAsset::Compile(bool8_t Force)
{
	if (Force || IsDirty() || !GetCache().Contains(ComputeHash()))
	{
		ReadRawData(AssetType::EContentsType::Text);
		ShaderCompileService::Get().Compile(*this);
		FreeRawData();
	}
}
