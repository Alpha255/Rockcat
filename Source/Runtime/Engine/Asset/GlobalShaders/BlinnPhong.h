#include "Assets/Shaders/Definitions.h"
#include "Runtime/Engine/Asset/MaterialAsset.h"
#include "Runtime/Engine/Asset/ShaderAsset.h"

class GenericVS : public ShaderAsset
{
public:
	GenericVS()
		: ShaderAsset("GenericVS.vert")
	{
	}
	DECLARE_GLOBAL_GENERIC_VS_VARIABLES
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

template<class FS, class VS = GenericVS>
class BaseMaterial : public MaterialAsset, public VS, public FS
{
public:
	BaseMaterial(const char8_t* MaterialAssetName)
		: MaterialAsset(MaterialAssetName)
		, m_Name(MaterialAssetName)
	{
		assert(MaterialAssetName);

		VS::ShaderVariables::AddShaderVariables(*this, *this);
		FS::ShaderVariables::AddShaderVariables(*this, *this);
	}

	virtual ~BaseMaterial()
	{
		for (auto& Property : m_Properties)
		{
			Property.second.Value = Property.second.Getter();
		}
		Save(true);
	}

	const char8_t* GetName() const { return m_Name.c_str(); }
	void SetName(const char8_t* Name) { m_Name = Name; }

	void CreateInstance();

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_BASE(MaterialAsset)
		);
	}
private:
	std::string m_Name;
};

class MaterialLit : public BaseMaterial<BlinnPhongFS>
{
public:
	MaterialLit()
		: BaseMaterial("Lit")
	{
	}

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_BASE(BaseMaterial)
		);
	}
};