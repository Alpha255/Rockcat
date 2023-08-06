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