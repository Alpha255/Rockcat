#pragma once

#include "Assets/Shaders/Definitions.h"
#include "Engine/Asset/Shader.h"

class GenericVS : public GlobalShader<GenericVS>
{
	DEFINITION_GENERIC_VS_SHADER_VARIABLES;
};

class DepthOnlyFS : public GlobalShader<DepthOnlyFS>
{
	DEFINITION_DEPTH_ONLY_FS_SHADER_VARIABLES;
};