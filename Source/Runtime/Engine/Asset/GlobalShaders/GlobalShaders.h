#pragma once

#include "Assets/Shaders/Definitions.h"
#include "Engine/Asset/MaterialAsset.h"

class ShaderBaseMetaData
{
public:
	ShaderBaseMetaData(const char* const SourceFile, const char* const Name, const char* const Entry, ERHIShaderStage Stage)
		: m_SourceFilePath(SourceFile)
		, m_Name(Name)
		, m_Entry(Entry)
		, m_Stage(Stage)
	{
	}

	const char* const GetSourceFilePath() const { return m_SourceFilePath; }
	const char* const GetName() const { return m_Name; }
	const char* const GetEntryPoint() const { return m_Entry; }
	ERHIShaderStage GetStage() const { return m_Stage; }
private:
	const char* const m_SourceFilePath;
	const char* const m_Name;
	const char* const m_Entry;
	ERHIShaderStage m_Stage;
};

template<class TShader>
class ShaderMetaData : public ShaderBaseMetaData
{
public:
	using ShaderBaseMetaData::ShaderBaseMetaData;
};

#define IMPLEMENT_SHADER_METADATA(ShaderClass, SourceFile, Entry, Stage) 

class MaterialShader
{
};

class ShaderLibrary
{
public:
	template<class T>
	void Register()
	{
	}

	template<class T>
	T* GetShader() 
	{
	}
};

class FShader : public ShaderDefines
{
public:
	enum class ECompileStatus
	{
		None,
		Compiling,
		Compiled
	};

	const std::map<std::string, ShaderVariable>& GetVariables() const { return m_Variables; }
	RHIBuffer* GetUniformBuffer(RHIDevice& Device);
	virtual void SetupViewParams(const class SceneView&) {}
	virtual void SetupMaterialProperty(const struct MaterialProperty&) {}
protected:
private:
	void RegisterVariable(const char* Name, ShaderVariable&& Variable);
	size_t ComputeUniformBufferSize();

	std::map<std::string, ShaderVariable> m_Variables;
	RHIBufferPtr m_UniformBuffer;
};

template<class T>
class TShader : public FShader
{
public:
	TShader()
	{
		T::RegisterShaderVariables(*this);
	}
};

struct DefaultVSShaderVariableContainer : public ShaderVariableContainer                                                                                                                 
{
	DEFINITION_DEFAULT_VS_SHADER_VARIABLES;

	DefaultVSShaderVariableContainer()
	{
		RegisterShaderVariables(*this);
	}

	void SetupViewParams(const SceneView& View) override final
	{
	}
};

class DefaultVS : public Shader<DefaultVSShaderVariableContainer>
{
public:
	DefaultVS()
		: Shader("GenericVS.vert")
	{
	}
};

struct DepthOnlyFSShaderVariableContainer : public ShaderVariableContainer
{
	DEFINITION_DEPTH_ONLY_SHADER_VARIABLES;

	DepthOnlyFSShaderVariableContainer()
	{
		RegisterShaderVariables(*this);
	}

	void SetupMaterialProperties(const MaterialProperty& Property) override final
	{
	}
};

class DepthOnlyFS : public Shader<DepthOnlyFSShaderVariableContainer>
{
public:
	DepthOnlyFS()
		: Shader("DepthOnly.frag")
	{
	}
};