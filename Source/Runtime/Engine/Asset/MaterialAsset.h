#pragma once

#include "Core/Math/Matrix.h"
#include "Engine/Asset/SerializableAsset.h"
#include "Engine/Asset/ImageAsset.h"
#include "Engine/RHI/RHIResource.h"

enum class EShadingMode
{
	Unlit,
	BlinnPhong,
	StandardPBR,
	Toon
};

using ShaderVariant = std::variant<
	std::monostate,
	float, int32_t, uint32_t,
	Math::Vector2,
	Math::Vector3,
	Math::Vector4,
	Math::Matrix,
	std::shared_ptr<ImageAsset>>;


struct MaterialProperty
{
	using SetFunc = std::function<void(const ShaderVariant&)>;
	using GetFunc = std::function<ShaderVariant(void)>;

	ERHIResourceType Type;
	uint32_t Binding;
	size_t Offset;
	size_t Stride;

	SetFunc Setter;
	GetFunc Getter;

	ShaderVariant Value;

	bool IsValueValid() const { return Value.index() != 0u; }

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
	template<class T>
	MaterialAsset(T&& Name)
		: BaseClass(GetFilePath(Name, GetExtension()))
	{
	}
	virtual ~MaterialAsset() = default;

	static const char* GetExtension() { return ".material"; }

	EShadingMode GetShadingMode() const { return m_ShadingMode; }

	void RegisterProperty(const char* InName, MaterialProperty&& Property)
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
			CEREAL_BASE(BaseClass),
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
