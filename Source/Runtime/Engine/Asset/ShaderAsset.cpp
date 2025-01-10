#include "Engine/Asset/ShaderAsset.h"
#include "Engine/Asset/ShaderCompiler.h"
#include "Engine/Services/ShaderCompileService.h"
#include "Engine/RHI/RHIInterface.h"
#include "Engine/Paths.h"

ERHIShaderStage ShaderAsset::GetStageByExtension(const std::filesystem::path& Extension)
{
	auto Ext = StringUtils::Lowercase(Extension.string());
	if (Ext == ".vert")
	{
		return ERHIShaderStage::Vertex;
	}
	else if (Ext == ".hull")
	{
		return ERHIShaderStage::Hull;
	}
	else if (Ext == ".domain")
	{
		return ERHIShaderStage::Domain;
	}
	else if (Ext == ".geom")
	{
		return ERHIShaderStage::Geometry;
	}
	else if (Ext == ".frag")
	{
		return ERHIShaderStage::Fragment;
	}
	else if (Ext == ".comp")
	{
		return ERHIShaderStage::Compute;
	}
	return ERHIShaderStage::Num;
}

void ShaderAsset::Compile(ERenderHardwareInterface RHI, bool Force)
{
	if (Force || IsDirty())
	{
		ShaderCompileService::Get().Compile(*this, RHI);
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
