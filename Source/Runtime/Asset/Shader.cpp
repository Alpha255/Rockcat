#include "Asset/Shader.h"
#include "RHI/RHIDevice.h"
#include "Paths.h"

ShaderBinary::ShaderBinary(const Shader& InShader, ERHIDeviceType DeviceType)
	: BaseClass(GetPath(InShader, DeviceType))
{
}

std::filesystem::path ShaderBinary::GetPath(const Shader& InShader, ERHIDeviceType DeviceType)
{
	const std::filesystem::path RelativePath = std::filesystem::relative(InShader.GetPath(), Paths::ShaderPath()).parent_path();
	return Paths::ShaderBinaryPath() / 
		RelativePath / 
		StringUtils::Format("%s_%s_%lld", InShader.GetStem(), RHIDevice::GetName(DeviceType), std::hash<Shader>()(InShader));
}

void Shader::RegisterVariable(const char* Name, ShaderVariable&& Variable)
{
	auto It = m_Variables.find(Name);
	if (It != m_Variables.end())
	{
		It->second = std::move(Variable);

		if (It->second.IsValid())
		{
			It->second.Set(It->second.Value);
		}
	}
	else
	{
		m_Variables.insert(std::make_pair(std::string_view(Name), std::forward<ShaderVariable>(Variable)));
	}
}
