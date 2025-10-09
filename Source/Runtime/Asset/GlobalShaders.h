#pragma once

#include "Assets/Shaders/Definitions.h"
#include "Asset/Shader.h"
#include "Paths.h"

class GenericVS : public GlobalShader<GenericVS>
{
public:
	GenericVS()
		: GlobalShader("GenericVS.vert", ERHIShaderStage::Vertex)
	{
	}

	DEFINE_SHADER_VARIABLES_GENERIC_VS;
};

class DepthOnlyFS : public GlobalShader<DepthOnlyFS>
{
	DepthOnlyFS()
		: GlobalShader("DepthOnly.frag", ERHIShaderStage::Fragment)
	{
	}

	DEFINE_SHADER_VARIABLES_DEPTH_ONLY_FS;
};