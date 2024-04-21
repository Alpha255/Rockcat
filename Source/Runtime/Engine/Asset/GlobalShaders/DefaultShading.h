#include "Assets/Shaders/Definitions.h"
#include "Engine/Asset/MaterialAsset.h"

//#pragma warning(push)
//#pragma warning(disable:4324)

class GenericVS : public ShaderAsset
{
public:
	GenericVS() : ShaderAsset("GenericVS.vert") {}

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
	DefaultUnlit() : ShaderAsset("DefaultUnlit.frag") {}

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
	DefaultLit() : ShaderAsset("DefaultLit.frag") {}

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
	DefaultToon() : ShaderAsset("DefaultToon.frag") {}

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
	DepthOnly() : ShaderAsset("DepthOnly.frag") {}

	DEFINITION_SHADER_VARIABLES_DEPTH_ONLY

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_BASE(ShaderAsset)
		);
	}
};

class MaterialUnlit : public Material<GenericVS, DefaultUnlit, MaterialUnlit>
{
public:
	using Material::Material;

	MaterialUnlit(MaterialID ID = MaterialID::NullIndex, const char* Name = nullptr)
		: Material(ID, EShadingMode::Unlit, Name ? Name : "DefaultUnlit")
	{
	}

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_BASE(Material)
		);
	}
private:
};

class MaterialLit : public Material<GenericVS, DefaultLit, MaterialLit>
{
public:
	using Material::Material;

	MaterialLit(MaterialID ID = MaterialID::NullIndex, EShadingMode ShadingMode = EShadingMode::BlinnPhong, const char* Name = nullptr)
		: Material(ID, ShadingMode, Name ? Name : "DefaultLit")
	{
		assert(ShadingMode == EShadingMode::BlinnPhong || ShadingMode == EShadingMode::StandardPBR);
		if (ShadingMode == EShadingMode::StandardPBR)
		{
			DefaultLit::SetDefine("_SHADING_MODE_STANDARD_PBR_", true);
		}
	}

	void SetShadingMode(EShadingMode ShadingMode)
	{ 
		assert(ShadingMode == EShadingMode::BlinnPhong || ShadingMode == EShadingMode::StandardPBR);
		Material::SetShadingMode(ShadingMode);
	}

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_BASE(Material)
		);
	}
};

class MaterialToon : public Material<GenericVS, DefaultToon, MaterialToon>
{
public:
	using Material::Material;

	MaterialToon(MaterialID ID = MaterialID::NullIndex, const char* Name = nullptr)
		: Material(ID, EShadingMode::Toon, Name ? Name : "DefaultToon")
	{
	}

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_BASE(Material)
		);
	}
};

//#pragma warning(pop)