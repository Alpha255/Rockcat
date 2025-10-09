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

uint32_t Shader::RegisterVariable(ShaderVariable&& Variable)
{
	uint32_t Index = static_cast<uint32_t>(m_Variables.size());
	m_Variables.emplace_back(std::forward<ShaderVariable>(Variable));
	return Index;
}
