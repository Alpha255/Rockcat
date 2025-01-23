#include "Engine/Asset/Shader.h"
#include "Engine/RHI/RHIDevice.h"

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
		m_Variables.insert(std::make_pair(std::string(Name), std::forward<ShaderVariable>(Variable)));
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
