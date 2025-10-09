#pragma once

#include "Assets/Shaders/Definitions.h"
#include "Asset/Shader.h"

class GenericVS : public GlobalShader<GenericVS>
{
	DEFINE_SHADER_VARIABLES_GENERIC_VS;
};

class DepthOnlyFS : public GlobalShader<DepthOnlyFS>
{
	DEFINE_SHADER_VARIABLES_DEPTH_ONLY_FS;
};