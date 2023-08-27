#pragma once

#include "Runtime/Core/Math/Matrix.h"
#include "Runtime/Engine/Asset/SerializableAsset.h"
#include "Runtime/Engine/Asset/ImageAsset.h"
#include "Runtime/Engine/RHI/RHIDeclarations.h"

enum class EShadingMode
{
	Unlit,
	BlinnPhong,
	StandardPBR,
	Toon
};

enum class EShaderVariableType : uint32_t
{
	Uniform,
	ImageSampler,
	RWBuffer
};

using ShaderVariant = std::variant<
	std::monostate,
	float32_t, int32_t, uint32_t,
	Math::Vector2,
	Math::Vector3,
	Math::Vector4,
	Math::Matrix,
	std::shared_ptr<ImageAsset>>;


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

	bool8_t IsValueValid() const { return Value.index() != 0u; }

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
		: ParentClass(Asset::GetPrefabricateAssetPath(MaterialAssetName, Asset::EPrefabAssetType::Material))
	{
	}
	virtual ~MaterialAsset() = default;

	const char8_t* GetExtension() const override final { return Asset::GetPrefabricateAssetExtension(Asset::EPrefabAssetType::Material); }

	EShadingMode GetShadingMode() const { return m_ShadingMode; }

	void RegisterProperty(const char8_t* InName, MaterialProperty&& Property)
	{
		std::string Name(InName);
		auto It = m_Properties.find(Name);
		if (It != m_Properties.end())
		{
			auto Value = It->second.Value;
			(*It).second = std::move(Property);
			if (It->second.IsValueValid())
			{
				It->second.Setter(Value);
			}
		}
		else
		{
			m_Properties.insert(std::make_pair(Name, std::forward<MaterialProperty>(Property)));
		}
	}

	virtual void Compile() {}

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_BASE(ParentClass),
			CEREAL_NVP(m_Properties)
		);
	}
protected:
	void SetShadingMode(EShadingMode ShadingMode) { m_ShadingMode = ShadingMode; }

	friend class AssimpSceneImporter;
	std::unordered_map<std::string, MaterialProperty> m_Properties;
	EShadingMode m_ShadingMode = EShadingMode::Unlit;
};

using MaterialID = ObjectID<MaterialAsset, uint32_t>;
