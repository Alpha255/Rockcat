#pragma once

#include "Assets/Shaders/Definitions.h"
#include "Engine/Asset/MaterialAsset.h"

struct DefaultVSShaderVariableContainer : public ShaderVariableContainer
{
	DEFINITION_DEFAULT_VS_SHADER_VARIABLES;

	DefaultVSShaderVariableContainer()
	{
		RegisterShaderVariables(*this);
	}

	void SetupViewParams(const IView& View) override final
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