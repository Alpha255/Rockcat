#include "Asset/Shader.h"
#include "RHI/RHIDevice.h"
#include "Paths.h"

ShaderBinary::ShaderBinary(const Shader& InShader, ERHIDeviceType DeviceType, ShaderBlob& Blob)
	: BaseClass(GetPath(InShader, DeviceType))
	, m_DeviceType(DeviceType)
	, m_ShaderLastWriteTime(InShader.GetLastWriteTime())
	, m_Blob(std::move(Blob))
{
	Save(true);
}

std::filesystem::path ShaderBinary::GetPath(const Shader& InShader, ERHIDeviceType DeviceType)
{
	const std::filesystem::path RelativePath = std::filesystem::relative(InShader.GetPath(), Paths::ShaderPath()).parent_path();
	return Paths::ShaderBinaryPath() / 
		RelativePath / 
		StringUtils::Format("%s_%s_%llu.json", 
			InShader.GetStem().c_str(), 
			RHIDevice::GetName(DeviceType), 
			std::hash<Shader>()(InShader));
}

size_t Shader::GetHash() const
{
	size_t Hash = std::hash<Shader>()(*this);
	return IsReady() ? ComputeHash(Hash, m_CachedRHI.get()) : Hash;
}

uint32_t Shader::RegisterVariable(ShaderVariable&& Variable)
{
	uint32_t Index = static_cast<uint32_t>(m_Variables.size());
	m_Variables.emplace_back(std::forward<ShaderVariable>(Variable));
	return Index;
}

const RHIShader* Shader::GetRHI(const RHIDevice& Device)
{
	if (m_CachedBinary)
	{
		if (m_CachedBinary->GetBlob().IsValid())
		{
			RHIShaderDesc Desc;
			Desc.SetName(GetStem())
				.SetStage(GetStage())
				.SetShaderBinary(m_CachedBinary.get());

			m_CachedRHI = Device.CreateShader(Desc);

			m_CachedBinary->ResetBlob();

			SetStatus(Asset::EStatus::Ready);
		}

		return m_CachedRHI.get();
	}

	return GetFallback(Device);
}

void Shader::SetBlob(ShaderBlob& Blob, ERHIDeviceType DeviceType)
{
	if (m_CachedBinary)
	{
#if _DEBUG
		if (m_CachedBinary->GetDeviceType() != DeviceType)
		{
			m_CachedBinary = std::make_shared<ShaderBinary>(*this, DeviceType, Blob);
			return;
		}
#endif
		m_CachedBinary->SetBlob(Blob, GetLastWriteTime());
		SetStatus(Asset::EStatus::Loading);
	}
	else
	{
		m_CachedBinary = std::make_shared<ShaderBinary>(*this, DeviceType, Blob);
	}
}

const RHIShader* Shader::GetFallback(const RHIDevice&) const
{
	return nullptr;
}
