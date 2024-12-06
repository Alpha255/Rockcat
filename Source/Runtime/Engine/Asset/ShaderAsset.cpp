#include "Engine/Asset/ShaderAsset.h"
#include "Engine/Asset/ShaderCompiler.h"
#include "Engine/Services/ShaderCompileService.h"
#include "Engine/RHI/RHIInterface.h"

class GlobalShaderCompileConfigurations : public SerializableAsset<GlobalShaderCompileConfigurations>
{
public:
	using BaseClass::BaseClass;

	static std::shared_ptr<GlobalShaderCompileConfigurations> Get()
	{
		static std::shared_ptr<GlobalShaderCompileConfigurations> s_Configs;
		if (!s_Configs)
		{
			s_Configs = GlobalShaderCompileConfigurations::Load(GetFilePath(ASSET_PATH_SHADERS, "GlobalShaderCompileConfigs", GetExtension()));
		}
		return s_Configs;
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

void ShaderAsset::SetupDefaultDefines()
{
	ShaderDefines::Merge(GlobalShaderCompileConfigurations::Get()->GetDefines(GetPath()));
}

const ShaderBinaryCache& ShaderAsset::GetCache() const
{
	if (!m_Cache)
	{
		m_Cache = ShaderBinaryCache::Load(GetFilePath(ASSET_PATH_SHADERCACHE, GetName(), ShaderBinaryCache::GetExtension()));
	}
	if (m_Cache->IsDirty())
	{
		m_Cache->Reload();
	}
	return *m_Cache;
}

ERHIShaderStage ShaderAsset::GetStage(const std::filesystem::path& Path)
{
	auto Extension = StringUtils::Lowercase(Path.extension().string());
	if (Extension == ".vert")
	{
		return ERHIShaderStage::Vertex;
	}
	else if (Extension == ".hull")
	{
		return ERHIShaderStage::Hull;
	}
	else if (Extension == ".domain")
	{
		return ERHIShaderStage::Domain;
	}
	else if (Extension == ".geom")
	{
		return ERHIShaderStage::Geometry;
	}
	else if (Extension == ".frag")
	{
		return ERHIShaderStage::Fragment;
	}
	else if (Extension == ".comp")
	{
		return ERHIShaderStage::Compute;
	}
	return ERHIShaderStage::Num;
}

void ShaderAsset::Compile(bool Force)
{
	if (Force || IsDirty() || !GetCache().Contains(ComputeHash()))
	{
		ReadRawData(AssetType::EContentsType::Text);
		ShaderCompileService::Get().Compile(*this);
		FreeRawData();
	}
}

size_t ShaderVariableContainer::ComputeUniformBufferSize()
{
	size_t UniformBufferSize = 0u;
	for (auto& [Name, Variable] : Variables)
	{
		if (Variable.Type == ERHIResourceType::UniformBuffer)
		{
			UniformBufferSize += Align(Variable.Stride, alignof(Math::Matrix));
		}
	}

	return UniformBufferSize;
}

void ShaderVariableContainer::RegisterVariable(const char* Name, ShaderVariable&& Variable)
{
	auto It = Variables.find(Name);
	if (It != Variables.end())
	{
		It->second = std::move(Variable);

		if (It->second.IsValid())
		{
			It->second.Set(It->second.Value);
		}
	}
	else
	{
		Variables.insert(std::make_pair(std::string(Name), std::forward<ShaderVariable>(Variable)));
	}
}
