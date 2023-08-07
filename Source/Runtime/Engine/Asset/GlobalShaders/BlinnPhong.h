#include "Assets/Shaders/Definitions.h"
#include "Runtime/Engine/Asset/MaterialAsset.h"
#include "Runtime/Engine/Asset/ShaderAsset.h"

class BlinnPhongVS : public ShaderAsset
{
public:
	BlinnPhongVS()
		: ShaderAsset("BlinnPhong.vert")
	{
	}
	DECLARE_GLOBAL_BLINN_PHONG_VS_VARIABLES
};

class BlinnPhongFS : public ShaderAsset
{
public:
	BlinnPhongFS()
		: ShaderAsset("BlinnPhong.frag")
	{
	}
	DECLARE_GLOBAL_BLINN_PHONG_FS_VARIABLES
};

class MaterialBlinnPhong : public MaterialAsset, public BlinnPhongVS, public BlinnPhongFS
{
public:
	MaterialBlinnPhong()
		: MaterialAsset("BlinnPhong")
	{
		BlinnPhongVS::ShaderVariables::AddShaderVariables(*this, *this);
		BlinnPhongFS::ShaderVariables::AddShaderVariables(*this, *this);
	}

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_BASE(MaterialAsset)
		);
	}
};