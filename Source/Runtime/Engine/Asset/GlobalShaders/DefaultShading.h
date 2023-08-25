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

	BaseMaterial(const char8_t* MaterialAssetName)
		: MaterialAsset(MaterialAssetName)
		, m_Name(MaterialAssetName)
	{
		assert(MaterialAssetName);

		VS::ShaderVariables::AddShaderVariables(*this, *this);
		FS::ShaderVariables::AddShaderVariables(*this, *this);
	}

	MaterialID GetID() const { return m_ID; }

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

	void CreateInstance(const char8_t* InstanceName);

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_BASE(MaterialAsset),
			CEREAL_NVP(m_Name),
			CEREAL_NVP(m_CullMode),
			CEREAL_NVP(m_DoubleSided),
			CEREAL_NVP(m_ID)
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
private:
	std::string m_Name;
	ERHICullMode m_CullMode = ERHICullMode::BackFace;
	bool8_t m_DoubleSided = false;
	MaterialID m_ID = ~0;
};

class MaterialUnlit : public BaseMaterial<DefaultUnlitFS>
{
public:
	MaterialUnlit()
		: BaseMaterial("DefaultUnlit")
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
	MaterialLit()
		: BaseMaterial("DefaultLit")
	{
	}

	EShadingMode GetShadingMode() const { return m_ShadingMode; }
	void SetShadingMode(EShadingMode ShadingMode) { m_ShadingMode = ShadingMode; }

	~MaterialLit()
	{
		SerializeProperties();
		Save<MaterialLit>(true);
	}

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_BASE(BaseMaterialType),
			CEREAL_NVP(m_ShadingMode)
		);
	}
private:
	EShadingMode m_ShadingMode = EShadingMode::BlinnPhong;
};

class MaterialToon : public BaseMaterial<DefaultToonFS>
{
public:
	MaterialToon()
		: BaseMaterial("DefaultToon")
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