#pragma once

#include "Colorful/IRenderer/Declarations.h"

NAMESPACE_START(RHI)

enum class EShaderParamterType : uint8_t
{
    UniformBuffer,
    Sampler,
    ShaderResourceView,
    UnorderedAccessView,
    Num
};

struct ShaderParameterInfo
{
    uint16_t Index = 0u;
    uint16_t Size = 0u;
    EShaderParamterType Type = EShaderParamterType::Num;
};

class ShaderCompilerDefinitions
{
public:
    void SetDefine(const char8_t* Name, const char8_t* Value)
    {
        m_Definitions.insert(std::make_pair(std::string(Name), std::string(Value)));
    }

    void SetDefine(const char8_t* Name, const std::string& Value)
    {
        m_Definitions.insert(std::make_pair(std::string(Name), Value));
    }

    void SetDefine(const char8_t* Name, bool8_t Value)
    {
        m_Definitions.insert(std::make_pair(std::string(Name), Value ? std::string("1") : std::string("0")));
    }

    void SetDefine(const char8_t* Name, int32_t Value)
    {
        m_Definitions.insert(std::make_pair(std::string(Name), String::Format("%d", Value)));
    }

    void SetDefine(const char8_t* Name, uint32_t Value)
    {
        m_Definitions.insert(std::make_pair(std::string(Name), String::Format("%d", Value)));
    }

    void SetDefine(const char8_t* Name, float32_t Value)
    {
        m_Definitions.insert(std::make_pair(std::string(Name), String::Format("%.2f", Value)));
    }

    void Merge(const ShaderCompilerDefinitions& Other)
    {
        m_Definitions.insert(Other.m_Definitions.begin(), other.m_Definitions.end());
    }

    const std::unordered_map<std::string, std::string>& GetDefinitions() const
    {
        return m_Definitions;
    }
protected:
private:
    std::unordered_map<std::string, std::string> m_Definitions;
};

NAMESPACE_END(RHI)