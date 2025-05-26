#include "Asset/Shader.h"
#include "RHI/RHIDevice.h"
#include "RHI/RHIBackend.h"
#include "Services/ShaderLibrary.h"

ShaderBinary::ShaderBinary(const std::string& ShaderName, ERHIBackend Backend, ERHIShaderStage Stage, std::time_t Timestamp, size_t Hash, ShaderBlob&& Blob)
	: BaseClass(GetUniquePath(ShaderName, Backend, Hash))
	, m_Backend(Backend)
	, m_Stage(Stage)
	, m_Timestamp(Timestamp)
	, m_Hash(Hash)
	, m_Blob(std::move(Blob))
{
}

std::filesystem::path ShaderBinary::GetUniquePath(const std::string& ShaderName, ERHIBackend Backend, size_t Hash)
{
	return Paths::ShaderBinaryCachePath() / RHIBackend::GetName(Backend) / StringUtils::Format("%s_%lld", ShaderName.c_str(), Hash);
}

RHIBuffer* Shader::GetUniformBuffer(RHIDevice& Device)
{
	if (!m_UniformBuffer)
	{
		RHIBufferCreateInfo CreateInfo;
		CreateInfo.SetUsages(ERHIBufferUsageFlags::UniformBuffer)
			.SetAccessFlags(ERHIDeviceAccessFlags::GpuReadCpuWrite)
			.SetSize(ComputeUniformBufferSize())
			.SetPermanentStates(ERHIResourceState::UniformBuffer);

		m_UniformBuffer = Device.CreateBuffer(CreateInfo);
	}

	return m_UniformBuffer.get();
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

size_t Shader::ComputeUniformBufferSize()
{
	size_t Size = 0u;
	for (auto& [Name, Variable] : m_Variables)
	{
		if (Variable.Type == ERHIResourceType::UniformBuffer)
		{
			Size += Align(Variable.Stride, alignof(Math::Matrix));
		}
	}

	Size = Align(Size, sizeof(Math::Vector4));

	return Size;
}

const RHIShader* Shader::TryGetRHI()
{
	if (!m_ShaderModule || GetMetaData().IsDirty())
	{
		m_ShaderModule = ShaderLibrary::Get().GetShaderModule(*this);
	}

	return m_ShaderModule ? m_ShaderModule : GetFallback();
}

const RHIShader* Shader::GetFallback() const
{
	switch (GetStage())
	{
	case ERHIShaderStage::Vertex:
		return nullptr;
	case ERHIShaderStage::Fragment:
		return nullptr;
	default:
		return nullptr;
	}
}
