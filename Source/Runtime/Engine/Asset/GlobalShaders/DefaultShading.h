#include "Assets/Shaders/Definitions.h"
#include "Engine/Asset/MaterialAsset.h"

//#pragma warning(push)
//#pragma warning(disable:4324)

class GenericVS : public ShaderAsset
{
public:
	GenericVS() 
		: ShaderAsset("GenericVS.vert") 
	{
		RegisterShaderVariables(*this);
	}

	DEFINITION_SHADER_VARIABLES_GENERIC_VS

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_BASE(ShaderAsset)
		);
	}
};

class DefaultUnlit : public ShaderAsset
{
public:
	DefaultUnlit() 
		: ShaderAsset("DefaultUnlit.frag") 
	{
		RegisterShaderVariables(*this);
	}

	DEFINITION_SHADER_VARIABLES_UNLIT

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_BASE(ShaderAsset)
		);
	}
};

class DefaultLit : public ShaderAsset
{
public:
	DefaultLit() 
		: ShaderAsset("DefaultLit.frag") 
	{
		RegisterShaderVariables(*this);
	}

	DEFINITION_SHADER_VARIABLES_LIT

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_BASE(ShaderAsset)
		);
	}
};

class DefaultToon : public ShaderAsset
{
public:
	DefaultToon() 
		: ShaderAsset("DefaultToon.frag") 
	{
		RegisterShaderVariables(*this);
	}

	DEFINITION_SHADER_VARIABLES_TOON

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_BASE(ShaderAsset)
		);
	}
};

class DepthOnly : public ShaderAsset
{
public:
	DepthOnly() 
		: ShaderAsset("DepthOnly.frag") 
	{
		RegisterShaderVariables(*this);
	}

	DEFINITION_SHADER_VARIABLES_DEPTH_ONLY

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_BASE(ShaderAsset)
		);
	}
};

class GenericShadow : public ShaderAsset
{
public:
	GenericShadow() 
		: ShaderAsset("GenericShadow.frag") 
	{
		//RegisterShaderVariables(*this);
	}

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_BASE(ShaderAsset)
		);
	}
};

//#pragma warning(pop)