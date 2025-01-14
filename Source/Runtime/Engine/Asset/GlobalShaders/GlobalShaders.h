#pragma once

#include "Assets/Shaders/Definitions.h"
#include "Engine/Asset/MaterialAsset.h"

class FShader
{
public:
};

class ShaderBaseMetaData
{
public:
	ShaderBaseMetaData(std::filesystem::path&& SourceFile, const char* const Name, const char* const Entry, ERHIShaderStage Stage)
		: m_SourceFilePath(std::move(SourceFile))
		, m_Name(Name)
		, m_Entry(Entry)
		, m_Stage(Stage)
	{
	}

	std::filesystem::path GetSourceFilePath() const { return m_SourceFilePath; }
	const char* const GetName() const { return m_Name; }
	const char* const GetEntryPoint() const { return m_Entry; }
	ERHIShaderStage GetStage() const { return m_Stage; }
private:
	std::filesystem::path m_SourceFilePath;
	const char* const m_Name;
	const char* const m_Entry;
	ERHIShaderStage m_Stage;
};

template<class TShader, class TFallbackShader>
class ShaderMetaData : public ShaderBaseMetaData
{
public:
	using ShaderBaseMetaData::ShaderBaseMetaData;
};

#define IMPLEMENT_SHADER_METADATA(ShaderClass, FallbackShaderClass, SourceFile, Entry, Stage) 

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