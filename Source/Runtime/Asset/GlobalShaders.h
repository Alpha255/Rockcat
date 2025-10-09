#pragma once

#include "Assets/Shaders/Definitions.h"
#include "Asset/Shader.h"
#include "Paths.h"

class GenericVS : public GlobalShader<GenericVS>
{
public:
	GenericVS()
		: GlobalShader(Paths::ShaderPath() / "GenericVS.vert", ERHIShaderStage::Vertex, "main")
	{
	}

	DEFINE_SHADER_VARIABLES_GENERIC_VS;
};

class DepthOnlyFS : public GlobalShader<DepthOnlyFS>
{
	DepthOnlyFS()
		: GlobalShader(Paths::ShaderPath() / "DepthOnly.frag", ERHIShaderStage::Fragment, "main")
	{
	}

	DEFINE_SHADER_VARIABLES_DEPTH_ONLY_FS;
};