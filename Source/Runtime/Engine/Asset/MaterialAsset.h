#pragma once

#include "Runtime/Engine/Asset/SerializableAsset.h"

#define BEGIN_SHADER_PARAMETERS_STRUCT struct alignas(16) ShaderParameters {
#define END_SHADER_PARAMETERS_STRUCT };

#define DECLARE_SHADER_VARIABLE(BaseType, VariableName, VariableType) \
	public: \
		BaseType VariableName; \
	private: \
		struct VariableID##VariableName {};	\
		static AppendShaderVariableFuncPtr AppendShaderVariable(size_t Stride, const char8_t* Name, VariableType Type, std::vector<ShaderVariable>& Variables) \
		{ \
		}

enum class ERHIShaderVariableType : uint32_t
{
	Uniform,
	ImageView,
	ImageSampler,
	RWBuffer
};

class ShaderVariable
{
public:
};

class MaterialAsset : public SerializableAsset<MaterialAsset>
{
public:
	template<class StringType>
	MaterialAsset(StringType&& MaterialAssetName)
		: ParentClass(Asset::GetPrefabricateAssetPath(MaterialAssetName, Asset::EPrefabricateAssetType::MaterialAsset))
	{
	}

	const char8_t* GetExtension() const override final { return Asset::GetPrefabricateAssetExtension(Asset::EPrefabricateAssetType::MaterialAsset); }

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_BASE(ParentClass)
		);
	}
private:
};

class GenericVS
{
public:
	BEGIN_SHADER_PARAMETERS_STRUCT
		Math::Matrix World;
		Math::Matrix View;
		Math::Matrix Projection;
	END_SHADER_PARAMETERS_STRUCT
};

class BlinnPhongFS
{
public:
	struct Parameters
	{
	};
};

class StandardPBRFS
{
public:
	struct Parameters
	{
	};
};

class ToonFS
{
public:
	struct Parameters
	{
	};
};

template<class ShadingModeFS>
class Material : public MaterialAsset
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
	GenericVS m_VertexShader;
	ShadingModeFS m_FragmentShader;
};

using BlinnPhongMaterial = Material<BlinnPhongFS>;
using StandardPBRMaterial = Material<StandardPBRFS>;
