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

	BEGIN_SHADER_DEFINES
		SHADER_DEFINE(bool, _HAS_NORMAL_, HasNormal)
		SHADER_DEFINE(bool, _HAS_TANGENT_, HasTangent)
		SHADER_DEFINE(bool, _HAS_UV0_, HasUV0)
		SHADER_DEFINE(bool, _HAS_UV1_, HasUV1)
		SHADER_DEFINE(bool, _HAS_COLOR_, HasColor)
	END_SHADER_DEFINES
};

class DepthOnlyFS : public GlobalShader<DepthOnlyFS>
{
public:
	DepthOnlyFS()
		: GlobalShader("DepthOnly.frag", ERHIShaderStage::Fragment)
	{
	}

	DEFINE_SHADER_VARIABLES_DEPTH_ONLY_FS;

	BEGIN_SHADER_DEFINES
		SHADER_DEFINE(bool, _HAS_UV0_, HasUV0)
	END_SHADER_DEFINES
};