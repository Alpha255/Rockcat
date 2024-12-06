#ifndef __INCLUDE_DEFINITIONS__
#define __INCLUDE_DEFINITIONS__

#ifdef __cplusplus

#include "Runtime/Core/Math/Matrix.h"
#include "Runtime/Engine/Asset/TextureAsset.h"

using uint = uint32_t;
using float2 = Math::Vector2;
using float3 = Math::Vector3;
using float4 = Math::Vector4;
using float4x4 = Math::Matrix;

#define DECLARE_SHADER_VARIABLES_BEGIN(Owner) \
protected: \
	using OwnerClass = Owner; \
private: \
	struct FirstVariableID{}; \
	typedef void* FuncPtr; \
	typedef FuncPtr(*RegisterShaderVariableFunc)(FirstVariableID, OwnerClass&); \
	static FuncPtr RegisterShaderVariable(FirstVariableID, OwnerClass&) { return nullptr; } \
	typedef FirstVariableID

#define DECLARE_SHADER_VARIABLE(Type, Name, RHIType, Binding, SetterGetter) \
	VariableID##Name; \
	private: \
		Type Name{}; \
	public: \
		SetterGetter \
	private: \
		struct NextVariableID_##Name{}; \
		static FuncPtr RegisterShaderVariable(NextVariableID_##Name, OwnerClass& Owner) \
		{ \
			ShaderVariable Variable{ ERHIResourceType::RHIType, Binding, offsetof(OwnerClass, Name), sizeof(Type) }; \
			Variable.Set = [&Owner](const ShaderVariable::Variant& Value) { Owner.##Name=std::get<Type>(Value);}; \
			Variable.Get = [&Owner](){ return ShaderVariable::Variant(Owner.##Name); }; \
			Owner.RegisterVariable(#Name, std::move(Variable)); \
			FuncPtr(*LastShaderVariableRegisterFunc)(VariableID##Name, OwnerClass&); \
			LastShaderVariableRegisterFunc = RegisterShaderVariable; \
			return reinterpret_cast<FuncPtr>(LastShaderVariableRegisterFunc); \
		} \
		typedef NextVariableID_##Name

#define DECLARE_SHADER_VARIABLES_END \
		LastVariableID; \
	public: \
		static void RegisterShaderVariables(OwnerClass& Owner) { \
			FuncPtr(*FinalRegisterShaderVariableFunc)(LastVariableID, OwnerClass& Owner); \
			FinalRegisterShaderVariableFunc = RegisterShaderVariable; \
			FuncPtr Func = reinterpret_cast<FuncPtr>(FinalRegisterShaderVariableFunc); \
			do \
			{ \
				Func = reinterpret_cast<RegisterShaderVariableFunc>(Func)(FirstVariableID(), Owner); \
			} while(Func); }

#define DECLARE_SV_UNIFORM_BUFFER_BEGIN(Binding) \
	NextVariableID_UniformBuffer; \
	private: \
		std::shared_ptr<RHIBuffer> UniformBuffer; \
	public: \
		const RHIBuffer* GetUniformBuffer() const { return UniformBuffer.get(); } \
	typedef NextVariableID_UniformBuffer \

#define DECLARE_SV_UNIFORM_BUFFER_END

#define DECLARE_SV_SETER_GETTER_DEFAULT(Type, Name) \
	inline const Type& Get##Name() const { return Name; } \
	inline OwnerClass& Set##Name(const Type&& Value) { Name = Value; return *this; } \

#define DECLARE_SV_SETER_GETTER_IMAGE_ASSET(Name) \
	inline const RHITexture* Get##Name() const { return Name->GetRHI(); } \
	template<class T> \
	OwnerClass& Set##Name(T&& Path) { Name = std::make_shared<TextureAsset>(std::forward<T>(Path)); return *this; }

#define DECLARE_SV_UNIFORM_BUFFER(Type, Name) DECLARE_SHADER_VARIABLE(Type, Name, UniformBuffer, ~0u, DECLARE_SV_SETER_GETTER_DEFAULT(Type, Name))

#define DECLARE_SV_TEXTURE_1D(Name, Binding) DECLARE_SHADER_VARIABLE(std::shared_ptr<TextureAsset>, Name, SampledImage, Binding, DECLARE_SV_SETER_GETTER_IMAGE_ASSET(Name))
#define DECLARE_SV_TEXTURE_ARRAY_1D(Name, Binding) DECLARE_SHADER_VARIABLE(std::shared_ptr<TextureAsset>, Name, SampledImage, Binding, DECLARE_SV_SETER_GETTER_IMAGE_ASSET(Name))

#define DECLARE_SV_TEXTURE_2D(Name, Binding) DECLARE_SHADER_VARIABLE(std::shared_ptr<TextureAsset>, Name, SampledImage, Binding, DECLARE_SV_SETER_GETTER_IMAGE_ASSET(Name))
#define DECLARE_SV_TEXTURE_ARRAY_2D(Name, Binding) DECLARE_SHADER_VARIABLE(std::shared_ptr<TextureAsset>, Name, SampledImage, Binding, DECLARE_SV_SETER_GETTER_IMAGE_ASSET(Name))

#define DECLARE_SV_TEXTURE_CUBE(Name, Binding) DECLARE_SHADER_VARIABLE(std::shared_ptr<TextureAsset>, Name, SampledImage, Binding, DECLARE_SV_SETER_GETTER_IMAGE_ASSET(Name))
#define DECLARE_SV_TEXTURE_ARRAY_CUBE(Name, Binding) DECLARE_SHADER_VARIABLE(std::shared_ptr<TextureAsset>, Name, SampledImage, Binding, DECLARE_SV_SETER_GETTER_IMAGE_ASSET(Name))

#define DECLARE_SV_TEXTURE_3D(Name, Binding) DECLARE_SHADER_VARIABLE(std::shared_ptr<TextureAsset>, Name, SampledImage, Binding, DECLARE_SV_SETER_GETTER_IMAGE_ASSET(Name))

#define DECLARE_SV_SAMPLER(Name, Binding)

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

#define MATH_PI       3.141592654f
#define MATH_PI_2     6.283185307f
#define MATH_PI_DIV2  1.570796327f
#define MATH_PI_DIV4  0.785398163f
#define MATH_PI_INV   0.318309886f
#define MATH_PI_2_INV 0.159154943f

// #pragma pack_matrix(row_major)

#define DECLARE_SHADER_VARIABLES_BEGIN(ShaderType)
#define DECLARE_SHADER_VARIABLE

#define DECLARE_SV_UNIFORM_BUFFER_BEGIN(Binding) cbuffer UniformBuffer : register(b##Binding) {
#define DECLARE_SV_UNIFORM_BUFFER_END };
#define DECLARE_SV_UNIFORM_BUFFER(Type, Name, SetterGetter) Type Name;

#define DECLARE_SHADER_VARIABLES_END

#define DECLARE_SV_TEXTURE_1D(Name, Binding) \
	Texture1D Name : register(t##Binding); \
	SamplerState Name##_Sampler : register(s##Binding);

#define DECLARE_SV_TEXTURE_2D(Name, Binding) \
	Texture2D Name : register(t##Binding); \
	SamplerState Name##_Sampler : register(s##Binding);

#define DECLARE_SV_TEXTURE_CUBE(Name, Binding) \
	Texture2D Name : register(t##Binding); \
	SamplerState Name##_Sampler : register(s##Binding);

struct VSInput
{
	VK_LOCATION(0) float3 Position : POSITION;

#if _HAS_NORMAL_
	VK_LOCATION(1) float3 Normal : NORMAL;
	#if _HAS_TANGENT_
		VK_LOCATION(2) float3 Tangent : TANGENT;
		VK_LOCATION(3) float3 BiTangent : BITANGENT;
		#if _HAS_UV0_
			VK_LOCATION(4) float3 UV0 : TEXCOORD0;
			#if _HAS_UV1_
				VK_LOCATION(5) float3 UV1 : TEXCOORD1;
				#if _HAS_COLOR_
					VK_LOCATION(6) float4 Color : COLOR;
				#endif
			#else
				#if _HAS_COLOR_
					VK_LOCATION(5) float4 Color : COLOR;
				#endif
			#endif
		#elif _HAS_UV1_
			VK_LOCATION(4) float3 UV1 : TEXCOORD1;
			#if _HAS_COLOR_
				VK_LOCATION(5) float4 Color : COLOR;
			#endif
		#else
			#if _HAS_COLOR_
				VK_LOCATION(4) float4 Color : COLOR;
			#endif
		#endif
	#else
		#if _HAS_UV0_
			VK_LOCATION(2) float3 UV0 : TEXCOORD0;
			#if _HAS_UV1_
				VK_LOCATION(3) float3 UV1 : TEXCOORD1;
				#if _HAS_COLOR_	
					VK_LOCATION(4) float4 Color : COLOR;
				#endif
			#else
				#if _HAS_COLOR_
					VK_LOCATION(3) float4 Color : COLOR;
				#endif
			#endif
		#elif _HAS_UV1_
			VK_LOCATION(2) float3 UV1 : TEXCOORD1;
			#if _HAS_COLOR_
				VK_LOCATION(3) float4 Color : COLOR;
			#endif
		#else
			#if _HAS_COLOR_
				VK_LOCATION(2) float4 Color : COLOR;
			#endif
		#endif
	#endif
#else
	#if _HAS_UV0_
		VK_LOCATION(1) float3 UV0 : TEXCOORD0;
		#if _HAS_UV1_
			VK_LOCATION(2) float3 UV1 : TEXCOORD1;
			#if _HAS_COLOR_
				VK_LOCATION(3) float4 Color : COLOR;
			#endif
		#else
			#if _HAS_COLOR_
				VK_LOCATION(2) float4 Color : COLOR;
			#endif
		#endif
	#elif _HAS_UV1_
		VK_LOCATION(1) float3 UV1 : TEXCOORD1;
		#if _HAS_COLOR_
			VK_LOCATION(2) float4 Color : COLOR;
		#endif
	#else
		#if _HAS_COLOR_
			VK_LOCATION(1) float4 Color : COLOR;
		#endif
	#endif
#endif
};

struct VSOutput
{
	float4 Position : SV_POSITION;
	VK_LOCATION(0) float3 WorldPosition : POSITIONT;

#if _HAS_NORMAL_
	VK_LOCATION(1) float3 WorldNormal : NORMAL;
	#if _HAS_TANGENT_
		VK_LOCATION(2) float3 WorldTangent : TANGENT;
		VK_LOCATION(3) float3 WorldBiTangent : BITANGENT;
		#if _HAS_UV0_
			VK_LOCATION(4) float3 UV0 : TEXCOORD0;
			#if _HAS_UV1_
				VK_LOCATION(5) float3 UV1 : TEXCOORD1;
				#if _HAS_COLOR_
					VK_LOCATION(6) float4 Color : COLOR;
				#endif
			#else
				#if _HAS_COLOR_
					VK_LOCATION(5) float4 Color : COLOR;
				#endif
			#endif
		#elif _HAS_UV1_
			VK_LOCATION(4) float3 UV1 : TEXCOORD1;
			#if _HAS_COLOR_
				VK_LOCATION(5) float4 Color : COLOR;
			#endif
		#else
			#if _HAS_COLOR_
				VK_LOCATION(4) float4 Color : COLOR;
			#endif
		#endif
	#else
		#if _HAS_UV0_
			VK_LOCATION(2) float3 UV0 : TEXCOORD0;
			#if _HAS_UV1_
				VK_LOCATION(3) float3 UV1 : TEXCOORD1;
				#if _HAS_COLOR_
					VK_LOCATION(4) float4 Color : COLOR;
				#endif
			#else
				#if _HAS_COLOR_
					VK_LOCATION(3) float4 Color : COLOR;
				#endif
			#endif
		#elif _HAS_UV1_
			VK_LOCATION(2) float3 UV1 : TEXCOORD1;
			#if _HAS_COLOR_
				VK_LOCATION(3) float4 Color : COLOR;
			#endif
		#else
			#if _HAS_COLOR_
				VK_LOCATION(2) float4 Color : COLOR;
			#endif
		#endif
	#endif
#else
	#if _HAS_UV0_
		VK_LOCATION(1) float3 UV0 : TEXCOORD0;
		#if _HAS_UV1_
			VK_LOCATION(2) float3 UV1 : TEXCOORD1;
			#if _HAS_COLOR_
				VK_LOCATION(3) float4 Color : COLOR;
			#endif
		#else
			#if _HAS_COLOR_
				VK_LOCATION(2) float4 Color : COLOR;
			#endif
		#endif
	#elif _HAS_UV1_
		VK_LOCATION(1) float3 UV1 : TEXCOORD1;
		#if _HAS_COLOR_
			VK_LOCATION(2) float4 Color : COLOR;
		#endif
	#else
		#if _HAS_COLOR_
			VK_LOCATION(1) float4 Color : COLOR;
		#endif
	#endif
#endif
};

#endif  // __cplusplus

#define DEFINITION_SHADER_VARIABLES_GENERIC_VS \
	DECLARE_SHADER_VARIABLES_BEGIN(GenericVS) \
		DECLARE_SV_UNIFORM_BUFFER_BEGIN(0) \
			DECLARE_SV_UNIFORM_BUFFER(float4x4, WorldMatrix) \
			DECLARE_SV_UNIFORM_BUFFER(float4x4, ViewMatrix) \
			DECLARE_SV_UNIFORM_BUFFER(float4x4, ProjectionMatrix) \
		DECLARE_SV_UNIFORM_BUFFER_END \
	DECLARE_SHADER_VARIABLES_END \

#define DEFINITION_SHADER_VARIABLES_UNLIT \
	DECLARE_SHADER_VARIABLES_BEGIN(DefaultUnlit) \
		DECLARE_SV_UNIFORM_BUFFER_BEGIN(1) \
		DECLARE_SV_UNIFORM_BUFFER_END \
		DECLARE_SV_TEXTURE_2D(BaseColorMap, 2) \
	DECLARE_SHADER_VARIABLES_END \

#define DEFINITION_SHADER_VARIABLES_LIT \
	DECLARE_SHADER_VARIABLES_BEGIN(DefaultLit) \
		DECLARE_SV_UNIFORM_BUFFER_BEGIN(1) \
		DECLARE_SV_UNIFORM_BUFFER_END \
		DECLARE_SV_TEXTURE_2D(BaseColorMap, 2) \
		DECLARE_SV_TEXTURE_2D(NormalMap, 3) \
		DECLARE_SV_TEXTURE_2D(MetallicRoughnessMap, 4) \
		DECLARE_SV_TEXTURE_2D(AOMap, 5) \
	DECLARE_SHADER_VARIABLES_END \

#define DEFINITION_SHADER_VARIABLES_DEPTH_ONLY \
	DECLARE_SHADER_VARIABLES_BEGIN(DepthOnly) \
		DECLARE_SV_UNIFORM_BUFFER_BEGIN(1) \
		DECLARE_SV_UNIFORM_BUFFER_END \
		DECLARE_SV_TEXTURE_2D(BaseColorMap, 2) \
	DECLARE_SHADER_VARIABLES_END \

#define DEFINITION_SHADER_VARIABLES_TOON \
	DECLARE_SHADER_VARIABLES_BEGIN(DefaultToon) \
		DECLARE_SV_UNIFORM_BUFFER_BEGIN(1) \
		DECLARE_SV_UNIFORM_BUFFER_END \
		DECLARE_SV_TEXTURE_2D(BaseColorMap, 2) \
	DECLARE_SHADER_VARIABLES_END \

#define DEFINITION_DEFAULT_VS_SHADER_VARIABLES \
	DECLARE_SHADER_VARIABLES_BEGIN(DefaultVSShaderVariableContainer) \
		DECLARE_SV_UNIFORM_BUFFER_BEGIN(0) \
			DECLARE_SV_UNIFORM_BUFFER(float4x4, WorldMatrix) \
			DECLARE_SV_UNIFORM_BUFFER(float4x4, ViewMatrix) \
			DECLARE_SV_UNIFORM_BUFFER(float4x4, ProjectionMatrix) \
		DECLARE_SV_UNIFORM_BUFFER_END \
	DECLARE_SHADER_VARIABLES_END \

#define DEFINITION_DEPTH_ONLY_SHADER_VARIABLES \
	DECLARE_SHADER_VARIABLES_BEGIN(DepthOnlyFSShaderVariableContainer) \
		DECLARE_SV_UNIFORM_BUFFER_BEGIN(1) \
		DECLARE_SV_UNIFORM_BUFFER_END \
		DECLARE_SV_TEXTURE_2D(BaseColorMap, 2) \
	DECLARE_SHADER_VARIABLES_END \

#endif  // __INCLUDE_DEFINITIONS__