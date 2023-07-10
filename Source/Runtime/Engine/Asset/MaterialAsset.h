#pragma once

#include "Runtime/Engine/Asset/SerializableAsset.h"

#define BEGIN_SHADER_PARAMETERS_STRUCT \
	struct alignas(16) ShaderParameters { \
	private: \
		struct FirstVariableID{}; \
		using ThisShaderParametersStruct = ShaderParameters; \
		typedef void* FuncPtr; \
		typedef FuncPtr(*AddShaderVariableFunc)(FirstVariableID, std::vector<ShaderVariable>&); \
		static FuncPtr AddShaderVariable(FirstVariableID, std::vector<ShaderVariable>&) { return nullptr; } \
		typedef FirstVariableID

#define END_SHADER_PARAMETERS_STRUCT \
		LastVariableID; \
	private: \
		static void AddShaderVariables(std::vector<ShaderVariable>& Variables) { \
			FuncPtr(*LastFunc)(LastVariableID, std::vector<ShaderVariable>& Variables); \
			LastFunc = AddShaderVariable; \
			FuncPtr Func = reinterpret_cast<FuncPtr>(LastFunc); \
			do \
			{ \
				Func = reinterpret_cast<AddShaderVariableFunc>(Func)(FirstVariableID(), Variables); \
			} while(Func); } \
		std::vector<ShaderVariable> m_Variables; \
	}; \

#define DECLARE_SHADER_VARIABLE(BaseType, VariableName, VariableType) \
	ThisVariableID##VariableName; \
	public: \
		BaseType VariableName; \
	private: \
		struct NextVariableID_##VariableName {}; \
		static FuncPtr AddShaderVariable(NextVariableID_##VariableName, std::vector<ShaderVariable>& Variables) \
		{ \
			Variables.emplace_back(ShaderVariable{ERHIShaderVariableType::VariableType, offsetof(ThisShaderParametersStruct, VariableName), sizeof(BaseType), #VariableName}); \
			FuncPtr(*PreFunc)(ThisVariableID##VariableName, std::vector<ShaderVariable>&); \
			PreFunc = AddShaderVariable; \
			return reinterpret_cast<FuncPtr>(PreFunc); \
		} \
		typedef NextVariableID_##VariableName

enum class ERHIShaderVariableType : uint32_t
{
	Uniform,
	ImageView,
	ImageSampler,
	RWBuffer
};

struct ShaderVariable
{
	ERHIShaderVariableType Type;
	size_t Offset;
	size_t Stride;
	const char8_t* Name;
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
		DECLARE_SHADER_VARIABLE(Math::Matrix, World, Uniform)
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
