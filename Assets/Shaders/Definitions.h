#ifndef __INCLUDE_DEFINITIONS__
#define __INCLUDE_DEFINITIONS__

#ifdef __cplusplus

#include "Runtime/Core/Math/Matrix.h"
#include "Runtime/Asset/TextureAsset.h"

using uint = uint32_t;
using float2 = Math::Vector2;
using float3 = Math::Vector3;
using float4 = Math::Vector4;
using float4x4 = Math::Matrix;

#define DECLARE_SHADER_VARIABLES_BEGIN(Owner) \
protected: \
	using OwnerClass = Owner; \
	struct UniformBufferBinding { inline static uint32_t Slot = 0u; }; \
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
			ShaderVariable Variable{ ERHIResourceType::RHIType, Binding, offsetof(OwnerClass, Name) - sizeof(Shader), sizeof(Type) }; \
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

#define DECLARE_SV_UNIFORM_BUFFER_BEGIN(Name, Binding) \
	VariableID##Name; \
	private: \
		struct NextVariableID_##Name{}; \
		static FuncPtr RegisterShaderVariable(NextVariableID_##Name, OwnerClass& Owner) \
		{ \
			UniformBufferBinding::Slot = Binding; \
			FuncPtr(*LastShaderVariableRegisterFunc)(VariableID##Name, OwnerClass&); \
			LastShaderVariableRegisterFunc = RegisterShaderVariable; \
			return reinterpret_cast<FuncPtr>(LastShaderVariableRegisterFunc); \
		} \
		typedef NextVariableID_##Name

#define DECLARE_SV_UNIFORM_BUFFER_END

#define DECLARE_SV_SETER_GETTER_DEFAULT(Type, Name) \
	inline const Type& Get##Name() const { return Name; } \
	inline OwnerClass& Set##Name(const Type&& Value) { Name = Value; return *this; } \

#define DECLARE_SV_SETER_GETTER_RESOURCE(Type, Name) \
	inline const Type* Get##Name() const { return Name; } \
	template<class T> \
	OwnerClass& Set##Name(const Type* Resource) { Name = Resource; return *this; }

#define DECLARE_SV_UNIFORM_BUFFER(Type, Name) DECLARE_SHADER_VARIABLE(Type, Name, UniformBuffer, UniformBufferBinding::Slot, DECLARE_SV_SETER_GETTER_DEFAULT(Type, Name))

#define DECLARE_SV_TEXTURE_1D(Name, Binding) DECLARE_SHADER_VARIABLE(const RHITexture*, Name, SampledImage, Binding, DECLARE_SV_SETER_GETTER_RESOURCE(RHITexture, Name))
#define DECLARE_SV_TEXTURE_ARRAY_1D(Name, Binding) DECLARE_SHADER_VARIABLE(const RHITexture*, Name, SampledImage, Binding, DECLARE_SV_SETER_GETTER_RESOURCE(RHITexture, Name))

#define DECLARE_SV_TEXTURE_2D(Name, Binding) DECLARE_SHADER_VARIABLE(const RHITexture*, Name, SampledImage, Binding, DECLARE_SV_SETER_GETTER_RESOURCE(RHITexture, Name))
#define DECLARE_SV_TEXTURE_ARRAY_2D(Name, Binding) DECLARE_SHADER_VARIABLE(const RHITexture*, Name, SampledImage, Binding, DECLARE_SV_SETER_GETTER_RESOURCE(RHITexture, Name))

#define DECLARE_SV_TEXTURE_CUBE(Name, Binding) DECLARE_SHADER_VARIABLE(const RHITexture*, Name, SampledImage, Binding, DECLARE_SV_SETER_GETTER_RESOURCE(RHITexture, Name))
#define DECLARE_SV_TEXTURE_ARRAY_CUBE(Name, Binding) DECLARE_SHADER_VARIABLE(const RHITexture*, Name, SampledImage, Binding, DECLARE_SV_SETER_GETTER_RESOURCE(RHITexture, Name))

#define DECLARE_SV_TEXTURE_3D(Name, Binding) DECLARE_SHADER_VARIABLE(const RHITexture*, Name, SampledImage, Binding, DECLARE_SV_SETER_GETTER_RESOURCE(RHITexture, Name))

#define DECLARE_SV_SAMPLER(Name, Binding) DECLARE_SHADER_VARIABLE(const RHISampler*, Name, Sampler, Binding, DECLARE_SV_SETER_GETTER_RESOURCE(RHISampler, Name))

#define DECLARE_SV_STORAGE_BUFFER(Name, Binding) DECLARE_SHADER_VARIABLE(const RHIBuffer*, Name, StorageBuffer, Binding, DECLARE_SV_SETER_GETTER_RESOURCE(RHIBuffer, Name)))

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

#define DECLARE_SV_UNIFORM_BUFFER_BEGIN(Name, Binding) cbuffer Name : register(b##Binding) {
#define DECLARE_SV_UNIFORM_BUFFER_END }
#define DECLARE_SV_UNIFORM_BUFFER(Type, Name) Type Name;

#define DECLARE_SHADER_VARIABLES_END

#define DECLARE_SV_TEXTURE_1D(Name, Binding) Texture1D Name : register(t##Binding);
#define DECLARE_SV_TEXTURE_2D(Name, Binding) Texture2D Name : register(t##Binding);
#define DECLARE_SV_TEXTURE_3D(Name, Binding) Texture3D Name : register(t##Binding);
#define DECLARE_SV_TEXTURE_CUBE(Name, Binding) Texture2D Name : register(t##Binding);

#define DECLARE_SV_SAMPLER(Name, Binding) SamplerState Name##_Sampler : register(s##Binding);

#define DECLARE_SV_STORAGE_BUFFER(Name, Binding) RWBuffer Name : register(u##Binding);

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

#define DEFINITION_GENERIC_VS_SHADER_VARIABLES \
	DECLARE_SHADER_VARIABLES_BEGIN(GenericVS) \
		DECLARE_SV_UNIFORM_BUFFER_BEGIN(WVP, 0) \
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

#define DEFINITION_DEPTH_ONLY_FS_SHADER_VARIABLES \
	DECLARE_SHADER_VARIABLES_BEGIN(DepthOnlyFS) \
		DECLARE_SV_UNIFORM_BUFFER_BEGIN(MaterialProperty, 1) \
			DECLARE_SV_UNIFORM_BUFFER(float, AlphaCutoff) \
		DECLARE_SV_UNIFORM_BUFFER_END \
		DECLARE_SV_TEXTURE_2D(BaseColorMap, 2) \
	DECLARE_SHADER_VARIABLES_END \

#define DEFINITION_SHADER_VARIABLES_TOON \
	DECLARE_SHADER_VARIABLES_BEGIN(DefaultToon) \
		DECLARE_SV_UNIFORM_BUFFER_BEGIN(1) \
		DECLARE_SV_UNIFORM_BUFFER_END \
		DECLARE_SV_TEXTURE_2D(BaseColorMap, 2) \
	DECLARE_SHADER_VARIABLES_END \

#endif  // __INCLUDE_DEFINITIONS__