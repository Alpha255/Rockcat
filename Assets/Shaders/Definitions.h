#ifndef __INCLUDE_DEFINITIONS__
#define __INCLUDE_DEFINITIONS__

#ifdef __cplusplus

#include "Runtime/Core/Math/Matrix.h"
#include "Runtime/Engine/Asset/ImageAsset.h"
using uint = uint32_t;
using float2 = Math::Vector2;
using float3 = Math::Vector3;
using float4 = Math::Vector4;
using float4x4 = Math::Matrix;

#define SHADER_VARIABLES_ALIGN_AS alignas(sizeof(Math::Vector4))

#define DECLARE_SHADER_VARIABLES_BEGIN(ShaderType) \
protected: \
	using ThisShader = ShaderType; \
	struct alignas(sizeof(Math::Vector4)) ShaderVariables { \
	private: \
	struct FirstVariableID{}; \
	using ThisShaderVariablesStruct = ShaderVariables; \
	typedef void* FuncPtr; \
	typedef FuncPtr(*AddShaderVariableFunc)(FirstVariableID, MaterialAsset&, ThisShader&); \
	static FuncPtr AddShaderVariable(FirstVariableID, MaterialAsset&, ThisShader&) { return nullptr; } \
	typedef FirstVariableID

#define DECLARE_SHADER_VARIABLE(BaseType, VariableName, VariableType, Binding, SetterGetter) \
	ThisVariableID##VariableName; \
	private: \
		BaseType VariableName{}; \
	public: \
		SetterGetter \
	private: \
		struct NextVariableID_##VariableName{}; \
		static FuncPtr AddShaderVariable(NextVariableID_##VariableName, MaterialAsset& Owner, ThisShader& Shader) \
		{ \
			MaterialProperty Property{ ERHIResourceType::VariableType, Binding, offsetof(ThisShaderVariablesStruct, VariableName), sizeof(BaseType) }; \
			Property.Setter = [&Shader](const ShaderVariant& Variant) { Shader.m_ShaderVariables.##VariableName=std::get<BaseType>(Variant);}; \
			Property.Getter = [&Shader](){ return ShaderVariant(Shader.m_ShaderVariables.##VariableName); }; \
			Owner.RegisterProperty(#VariableName, std::move(Property)); \
			FuncPtr(*PreFunc)(ThisVariableID##VariableName, MaterialAsset&, ThisShader&); \
			PreFunc = AddShaderVariable; \
			return reinterpret_cast<FuncPtr>(PreFunc); \
		} \
		typedef NextVariableID_##VariableName

#define DECLARE_SHADER_VARIABLES_END \
		LastVariableID; \
	public: \
		static void AddShaderVariables(MaterialAsset& Owner, ThisShader& Shader) { \
			FuncPtr(*LastFunc)(LastVariableID, MaterialAsset& Owner, ThisShader& Shader); \
			LastFunc = AddShaderVariable; \
			FuncPtr Func = reinterpret_cast<FuncPtr>(LastFunc); \
			do \
			{ \
				Func = reinterpret_cast<AddShaderVariableFunc>(Func)(FirstVariableID(), Owner, Shader); \
			} while(Func); } \
	}; \
	ShaderVariables m_ShaderVariables; \
public: \
	ShaderVariables& GetShaderVariables() { return m_ShaderVariables; } \
	const ShaderVariables& GetShaderVariables() const { return m_ShaderVariables; }

#define DECLARE_UNIFORM_BUFFER_BEGIN(Binding)
#define DECLARE_UNIFORM_BUFFER_END

#define DECLARE_SHADER_VARIABLE_SETER_GETTER_DEFAULT(BaseType, VariableName) \
	inline const BaseType& Get##VariableName() const { return VariableName; } \
	template<class Type> inline ShaderVariables& Set##VariableName(Type&& Value) { VariableName = std::forward<Type>(Value); return *this; } \

#define DECLARE_SHADER_VARIABLE_SETER_GETTER_BY_PATH(BaseType, VariableName) \
	inline const BaseType& Get##VariableName() const { return VariableName; } \
	template<class StringType> \
	void Set##VariableName(StringType&& Path) { VariableName = std::make_shared<ImageAsset>(std::forward<StringType>(Path)); }

#define DECLARE_SHADER_VARIABLE_UNIFORM(BaseType, VariableName, Binding) DECLARE_SHADER_VARIABLE(BaseType, VariableName, UniformBuffer, Binding, DECLARE_SHADER_VARIABLE_SETER_GETTER_DEFAULT(BaseType, VariableName))

#define DECLARE_SHADER_VARIABLE_TEXTURE_1D(VariableName, Binding) DECLARE_SHADER_VARIABLE(std::shared_ptr<ImageAsset>, VariableName, SampledImage, Binding, DECLARE_SHADER_VARIABLE_SETER_GETTER_BY_PATH(std::shared_ptr<ImageAsset>, VariableName))
#define DECLARE_SHADER_VARIABLE_TEXTURE_ARRAY_1D(VariableName, Binding) DECLARE_SHADER_VARIABLE(std::shared_ptr<ImageAsset>, VariableName, SampledImage, Binding, DECLARE_SHADER_VARIABLE_SETER_GETTER_BY_PATH(std::shared_ptr<ImageAsset>, VariableName))

#define DECLARE_SHADER_VARIABLE_TEXTURE_2D(VariableName, Binding) DECLARE_SHADER_VARIABLE(std::shared_ptr<ImageAsset>, VariableName, SampledImage, Binding, DECLARE_SHADER_VARIABLE_SETER_GETTER_BY_PATH(std::shared_ptr<ImageAsset>, VariableName))
#define DECLARE_SHADER_VARIABLE_TEXTURE_ARRAY_2D(VariableName, Binding) DECLARE_SHADER_VARIABLE(std::shared_ptr<ImageAsset>, VariableName, SampledImage, Binding, DECLARE_SHADER_VARIABLE_SETER_GETTER_BY_PATH(std::shared_ptr<ImageAsset>, VariableName))

#define DECLARE_SHADER_VARIABLE_TEXTURE_CUBE(VariableName, Binding) DECLARE_SHADER_VARIABLE(std::shared_ptr<ImageAsset>, VariableName, SampledImage, Binding, DECLARE_SHADER_VARIABLE_SETER_GETTER_BY_PATH(std::shared_ptr<ImageAsset>, VariableName))
#define DECLARE_SHADER_VARIABLE_TEXTURE_ARRAY_CUBE(VariableName, Binding) DECLARE_SHADER_VARIABLE(std::shared_ptr<ImageAsset>, VariableName, SampledImage, Binding, DECLARE_SHADER_VARIABLE_SETER_GETTER_BY_PATH(std::shared_ptr<ImageAsset>, VariableName))

#define DECLARE_SHADER_VARIABLE_TEXTURE_3D(VariableName, Binding) DECLARE_SHADER_VARIABLE(std::shared_ptr<ImageAsset>, VariableName, SampledImage, Binding, DECLARE_SHADER_VARIABLE_SETER_GETTER_BY_PATH(std::shared_ptr<ImageAsset>, VariableName))

#else  // __cplusplus

#if _SPIRV_
    #define VK_PUSH_CONSTANT      [[vk::push_constant]]
    #define VK_BINDING(Slot, Set) [[vk::binding(Slot, Set)]]
    #define VK_LOCATION(Index)    [[vk::location(Index)]]
    #define VK_OFFSET(Offset)     [[vk::offset(Offset)]]
#else  // _SPIRV_
    #define VK_PUSH_CONSTANT
    #define VK_BINDING(Slot, Set)
    #define VK_LOCATION(Index)
    #define VK_OFFSET(Offset)
#endif  // _SPIRV_

#define MATH_PI 3.141592654f
#define MATH_PI_2 6.283185307f
#define MATH_PI_DIV2 1.570796327f
#define MATH_PI_DIV4 0.785398163f
#define MATH_PI_INV 0.318309886f
#define MATH_PI_2_INV 0.159154943f

// #pragma pack_matrix(row_major)

#define DECLARE_SHADER_VARIABLES_BEGIN(ShaderType)
#define DECLARE_SHADER_VARIABLE
#define DECLARE_SHADER_VARIABLE_UNIFORM(BaseType, VariableName, Binding) BaseType VariableName;
#define DECLARE_UNIFORM_BUFFER_BEGIN(Binding) cbuffer UniformBuffer : register(b##Binding) {
#define DECLARE_UNIFORM_BUFFER_END };
#define DECLARE_SHADER_VARIABLES_END

#define DECLARE_SHADER_VARIABLE_TEXTURE_1D(TextureName, Binding) \
	Texture1D TextureName : register(t##Binding); \
	SamplerState TextureName##_Sampler : register(s##Binding);

#define DECLARE_SHADER_VARIABLE_TEXTURE_2D(TextureName, Binding) \
	Texture2D TextureName : register(t##Binding); \
	SamplerState TextureName##_Sampler : register(s##Binding);

#define DECLARE_SHADER_VARIABLE_TEXTURE_CUBE(TextureName, Binding) \
	Texture2D TextureName : register(t##Binding); \
	SamplerState TextureName##_Sampler : register(s##Binding);

struct VSInput
{
	VK_LOCATION(0) float3 Position : POSITION;
    VK_LOCATION(1) float3 Normal : NORMAL;
    
#if _HAS_TANGENT_
    VK_LOCATION(2) float3 Tangent : TANGENT;
	VK_LOCATION(3) float3 BiTangent : BITANGENT;
	#if _HAS_UV0_
		VK_LOCATION(4) float2 UV0 : TEXCOORD0;
		#if _HAS_UV1_
    		VK_LOCATION(5) float2 UV1 : TEXCOORD1;
			#if _HAS_COLOR_
    			VK_LOCATION(6) float4 Color : COLOR;
			#endif
		#else
			#if _HAS_COLOR_
				VK_LOCATION(5) float4 Color : COLOR;
			#endif
		#endif
	#else
		#if _HAS_COLOR_
    		VK_LOCATION(4) float4 Color : COLOR;
		#endif
	#endif
#else
	#if _HAS_UV0_
		VK_LOCATION(2) float2 UV0 : TEXCOORD0;
		#if _HAS_UV1_
    		VK_LOCATION(3) float2 UV1 : TEXCOORD1;
			#if _HAS_COLOR_	
    			VK_LOCATION(4) float4 Color : COLOR;
			#endif
		#else
			#if _HAS_COLOR_
				VK_LOCATION(3) float4 Color : COLOR;
			#endif
		#endif
	#else
		#if _HAS_COLOR_
    		VK_LOCATION(2) float4 Color : COLOR;
		#endif
	#endif
#endif
};

struct VSOutput
{
	float4 Position : SV_POSITION;
	VK_LOCATION(0) float3 WorldPosition : POSITIONT;
    VK_LOCATION(1) float3 WorldNormal : NORMAL;
    
#if _HAS_TANGENT_
    VK_LOCATION(2) float3 WorldTangent : TANGENT;
	VK_LOCATION(3) float3 WorldBiTangent : BITANGENT;
	#if _HAS_UV0_
		VK_LOCATION(4) float2 UV0 : TEXCOORD0;
		#if _HAS_UV1_
    		VK_LOCATION(5) float2 UV1 : TEXCOORD1;
			#if _HAS_COLOR_
    			VK_LOCATION(6) float4 Color : COLOR;
			#endif
		#else
			#if _HAS_COLOR_
				VK_LOCATION(5) float4 Color : COLOR;
			#endif
		#endif
	#else
		#if _HAS_COLOR_
    		VK_LOCATION(4) float4 Color : COLOR;
		#endif
	#endif
#else
	#if _HAS_UV0_
		VK_LOCATION(2) float2 UV0 : TEXCOORD0;
		#if _HAS_UV1_
    		VK_LOCATION(3) float2 UV1 : TEXCOORD1;
			#if _HAS_COLOR_
    			VK_LOCATION(4) float4 Color : COLOR;
			#endif
		#else
			#if _HAS_COLOR_
				VK_LOCATION(3) float4 Color : COLOR;
			#endif
		#endif
	#else
		#if _HAS_COLOR_
    		VK_LOCATION(3) float4 Color : COLOR;
		#endif
	#endif
#endif
};

#endif  // __cplusplus

#define DECLARE_GLOBAL_GENERIC_VS_VARIABLES \
	DECLARE_SHADER_VARIABLES_BEGIN(GenericVS) \
		DECLARE_UNIFORM_BUFFER_BEGIN(0) \
			DECLARE_SHADER_VARIABLE_UNIFORM(float4x4, WorldMatrix, 0) \
			DECLARE_SHADER_VARIABLE_UNIFORM(float4x4, ViewMatrix, 0) \
			DECLARE_SHADER_VARIABLE_UNIFORM(float4x4, ProjectionMatrix, 0) \
		DECLARE_UNIFORM_BUFFER_END \
	DECLARE_SHADER_VARIABLES_END \

#define DECLARE_GLOBAL_DEFAULT_UNLIT_FS_VARIABLES \
	DECLARE_SHADER_VARIABLES_BEGIN(DefaultUnlitFS) \
		DECLARE_UNIFORM_BUFFER_BEGIN(1) \
		DECLARE_UNIFORM_BUFFER_END \
		DECLARE_SHADER_VARIABLE_TEXTURE_2D(DiffuseMap, 2) \
	DECLARE_SHADER_VARIABLES_END \

#define DECLARE_GLOBAL_DEFAULT_LIT_FS_VARIABLES \
	DECLARE_SHADER_VARIABLES_BEGIN(DefaultLitFS) \
		DECLARE_UNIFORM_BUFFER_BEGIN(1) \
		DECLARE_UNIFORM_BUFFER_END \
		DECLARE_SHADER_VARIABLE_TEXTURE_2D(BaseColorMap, 2) \
		DECLARE_SHADER_VARIABLE_TEXTURE_2D(NormalMap, 3) \
		DECLARE_SHADER_VARIABLE_TEXTURE_2D(MetallicRoughnessMap, 4) \
		DECLARE_SHADER_VARIABLE_TEXTURE_2D(AOMap, 5) \
	DECLARE_SHADER_VARIABLES_END \

#define DECLARE_GLOBAL_DEFAULT_TOON_FS_VARIABLES \
	DECLARE_SHADER_VARIABLES_BEGIN(DefaultToonFS) \
		DECLARE_UNIFORM_BUFFER_BEGIN(1) \
		DECLARE_UNIFORM_BUFFER_END \
		DECLARE_SHADER_VARIABLE_TEXTURE_2D(BaseColorMap, 2) \
	DECLARE_SHADER_VARIABLES_END \

#endif  // __INCLUDE_DEFINITIONS__