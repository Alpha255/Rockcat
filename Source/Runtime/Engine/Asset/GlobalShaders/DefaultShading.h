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

class DefaultUnlitFS : public ShaderAsset
{
public:
	DefaultUnlitFS()
		: ShaderAsset("DefaultUnlit.frag")
	{
	}
	DECLARE_GLOBAL_DEFAULT_UNLIT_FS_VARIABLES
};

class DefaultLitFS : public ShaderAsset
{
public:
	DefaultLitFS()
		: ShaderAsset("DefaultLit.frag")
	{
	}
	DECLARE_GLOBAL_DEFAULT_LIT_FS_VARIABLES
};

class DefaultToonFS : public ShaderAsset
{
public:
	DefaultToonFS()
		: ShaderAsset("DefaultToon.frag")
	{
	}
	DECLARE_GLOBAL_DEFAULT_TOON_FS_VARIABLES
};

template<class FS, class VS = GenericVS>
class BaseMaterial : public MaterialAsset, public VS, public FS
{
public:
	using BaseMaterialType = BaseMaterial<FS, VS>;
	using MaterialAsset::MaterialAsset;

	BaseMaterial(MaterialID ID, EShadingMode ShadingMode, const char8_t* MaterialAssetName)
		: MaterialAsset(MaterialAssetName)
		, m_ID(ID)
		, m_Name(MaterialAssetName)
	{
		assert(MaterialAssetName);

		SetShadingMode(ShadingMode);

		VS::ShaderVariables::AddShaderVariables(*this, *this);
		FS::ShaderVariables::AddShaderVariables(*this, *this);
	}

	const MaterialID& GetID() const { return m_ID; }

	const char8_t* GetName() const { return m_Name.c_str(); }
	void SetName(const char8_t* Name) 
	{ 
		assert(Name);
		m_Name = Name; 
		SetPath(Asset::GetPrefabricateAssetPath(Name, Asset::EPrefabAssetType::Material));
	}

	ERHICullMode GetCullMode() const { return m_CullMode; }
	void SetCullMode(ERHICullMode CullMode) { m_CullMode = CullMode; }

	bool8_t IsDoubleSided() const { return m_DoubleSided; }
	void SetDoubleSided(bool8_t DoubleSided) { m_DoubleSided = DoubleSided; }

	bool8_t IsReady() const override final { return Asset::IsReady() && VS::IsReady() && FS::IsReady(); }

	void Compile() override final { VS::Compile(); FS::Compile(); }

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_BASE(MaterialAsset),
			CEREAL_NVP(m_CullMode),
			CEREAL_NVP(m_DoubleSided),
			CEREAL_NVP(m_ShadingMode),
			CEREAL_NVP(m_Name)
		);
	}
protected:
	void SerializeProperties()
	{
		for (auto& Property : m_Properties)
		{
			Property.second.Value = Property.second.Getter();
		}
	}

	static constexpr bool8_t IsValidName(const char8_t* Name) { return Name ? strlen(Name) > 0u : false; }

	void PostLoad() override final
	{
		VS::ShaderVariables::AddShaderVariables(*this, *this);
		FS::ShaderVariables::AddShaderVariables(*this, *this);
	}
private:
	MaterialID m_ID;
	ERHICullMode m_CullMode = ERHICullMode::BackFace;
	bool8_t m_DoubleSided = false;
	std::string m_Name;
};

class MaterialUnlit : public BaseMaterial<DefaultUnlitFS>
{
public:
	using BaseMaterial::BaseMaterial;

	MaterialUnlit(MaterialID ID = MaterialID::NullIndex, const char8_t* Name = nullptr)
		: BaseMaterial(ID, EShadingMode::Unlit, IsValidName(Name) ? Name : "DefaultUnlit")
	{
	}

	~MaterialUnlit()
	{
		SerializeProperties();
		Save<MaterialUnlit>(true);
	}

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_BASE(BaseMaterialType)
		);
	}
private:
};

class MaterialLit : public BaseMaterial<DefaultLitFS>
{
public:
	using BaseMaterial::BaseMaterial;

	MaterialLit(MaterialID ID = MaterialID::NullIndex, EShadingMode ShadingMode = EShadingMode::BlinnPhong, const char8_t* Name = nullptr)
		: BaseMaterial(ID, ShadingMode, IsValidName(Name) ? Name : "DefaultLit")
	{
		assert(ShadingMode == EShadingMode::BlinnPhong || ShadingMode == EShadingMode::StandardPBR);
		if (ShadingMode == EShadingMode::StandardPBR)
		{
			DefaultLitFS::SetDefine("_SHADING_PBR_", 1u);
		}
	}

	void SetShadingMode(EShadingMode ShadingMode)
	{ 
		assert(ShadingMode == EShadingMode::BlinnPhong || ShadingMode == EShadingMode::StandardPBR);
		BaseMaterial::SetShadingMode(ShadingMode);
	}

	~MaterialLit()
	{
		SerializeProperties();
		Save<MaterialLit>(true);
	}

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_BASE(BaseMaterialType)
		);
	}
};

class MaterialToon : public BaseMaterial<DefaultToonFS>
{
public:
	using BaseMaterial::BaseMaterial;

	MaterialToon(MaterialID ID = MaterialID::NullIndex, const char8_t* Name = nullptr)
		: BaseMaterial(ID, EShadingMode::Toon, IsValidName(Name) ? Name : "DefaultToon")
	{
	}

	~MaterialToon()
	{
		SerializeProperties();
		Save<MaterialToon>(true);
	}

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_BASE(BaseMaterialType)
		);
	}
private:
};