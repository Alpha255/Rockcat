#pragma once

#include "Runtime/Engine/Asset/SerializableAsset.h"
#include "Runtime/Engine/RHI/RHIShader.h"
#include "Runtime/Engine/RHI/RHIImage.h"

enum class EShadingMode
{
	Unlit,
	BlinnPhong,
	Toon,
	StandardPBR
};

struct ShaderMetaData
{
	ERHIShaderStage Stage;
	std::string SourceFilePath;
	std::string EntryName;

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_NVP(Stage),
			CEREAL_NVP(SourceFilePath),
			CEREAL_NVP(EntryName)
		);
	}
};

struct RHIImageVariable
{
	std::string ImagePath;
	RHIImage* Image = nullptr;

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_NVP(ImagePath)
		);
	}
};

using ShaderVariant = std::variant<
	float32_t, int32_t, uint32_t,
	Math::Vector2,
	Math::Vector3,
	Math::Vector4,
	Math::Matrix,
	RHIImageVariable>;


struct MaterialProperty
{
	using SetFunc = std::function<void(const ShaderVariant&)>;
	using GetFunc = std::function<ShaderVariant(void)>;

	EShaderVariableType Type;
	uint32_t Binding;
	size_t Offset;
	size_t Stride;

	SetFunc Setter;
	GetFunc Getter;

	ShaderVariant Value;

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_NVP(Value)
		);
	}
};

class MaterialAsset : public SerializableAsset<MaterialAsset>
{
public:
	template<class StringType>
	MaterialAsset(StringType&& MaterialAssetName)
		: ParentClass(Asset::GetPrefabricateAssetPath(MaterialAssetName, Asset::EPrefabricateAssetType::MaterialAsset))
	{
	}

	~MaterialAsset()
	{
		for (auto& Property : m_Properties)
		{
			Property.second.Setter(Property.second.Value);
		}

		Save(true);
	}

	const char8_t* GetExtension() const override final { return Asset::GetPrefabricateAssetExtension(Asset::EPrefabricateAssetType::MaterialAsset); }

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_BASE(ParentClass),
			CEREAL_NVP(m_Properties)
		);
	}

	void RegisterProperty(const char8_t* InName, MaterialProperty&& Property)
	{
		std::string Name(InName);
		auto It = m_Properties.find(Name);
		if (It != m_Properties.end())
		{
			It->second.Setter(It->second.Value);
		}
		else
		{
			m_Properties.insert(std::make_pair(Name, std::forward<MaterialProperty>(Property)));
		}
	}
protected:

private:
	std::unordered_map<std::string, MaterialProperty> m_Properties;
};

#define DECLARE_MATERIAL_PROPERTY(BaseType, PropertyName, DefaultValue) \
private: \
	BaseType m_#PropertyName = DefaultValue; \
public: \
	BaseType Get#PropertyName() const { return m_#PropertyName; } \
	void Set#PropertyName(const BaseType& Value) { m_#PropertyName = Value; }

class BaseMaterial : public MaterialAsset
{
public:
	using MaterialAsset::MaterialAsset;
};

using MaterialID = ObjectID<BaseMaterial>;
#if 0
template<class ShadingModeFS>
class Material : public MaterialAsset, public ShadingModeFS
{
public:
	using MaterialAsset::MaterialAsset;

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_BASE(MaterialAsset)
		);
	}
private:
};
#endif
