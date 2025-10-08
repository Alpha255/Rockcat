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

#define BEGIN_SHADER_PARAMETER(Owner) \
protected: \
	using OwnerClass = Owner; \
private: \
	struct FirstVariableID{}; \
	typedef void* FuncPtr; \
	typedef FuncPtr(*RegisterShaderVariableFunc)(FirstVariableID, OwnerClass&); \
	static FuncPtr RegisterShaderVariable(FirstVariableID, OwnerClass&) { return nullptr; } \
	typedef FirstVariableID

#define SHADER_PARAMETER(Type, Name, ResType, Binding, SetterGetter) \
	VariableID##Name; \
	private: \
		Type Name{}; \
	public: \
		SetterGetter \
	private: \
		struct NextVariableID_##Name{}; \
		static FuncPtr RegisterShaderVariable(NextVariableID_##Name, OwnerClass& Owner) \
		{ \
			ShaderVariable Variable{ ERHIResourceType::ResType, Binding }; \
			Variable.Set = [&Owner](const ShaderVariable::Variant& Value) { Owner.##Name=std::get<Type>(Value);}; \
			Variable.Get = [&Owner](){ return ShaderVariable::Variant(Owner.##Name); }; \
			Owner.RegisterVariable(#Name, std::move(Variable)); \
			FuncPtr(*LastShaderVariableRegisterFunc)(VariableID##Name, OwnerClass&); \
			LastShaderVariableRegisterFunc = RegisterShaderVariable; \
			return reinterpret_cast<FuncPtr>(LastShaderVariableRegisterFunc); \
		} \
		typedef NextVariableID_##Name

#define END_SHADER_PARAMETER \
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

#define SHADER_PARAMETER_SETTER_GETTER_DEFAULT(Type, Name) \
	inline const Type* Get##Name() const { return Name; } \
	template<class T> \
	OwnerClass& Set##Name(const Type* Resource) { Name = Resource; return *this; }

#define SHADER_PARAMETER_GETTER_UNIFORM_BUFFER(Type, Name) \
	inline UniformBuffer_##Type& GetUniformBuffer() { return Name; } \

#define SHADER_PARAMETER_UNIFORM_BUFFER(Type, Name, Binding) SHADER_PARAMETER(UniformBuffer_##Type, Name, UniformBuffer, Binding, SHADER_PARAMETER_GETTER_UNIFORM_BUFFER(Type, Name))

#define SHADER_PARAMETER_TEXTURE_1D(Name, Binding) SHADER_PARAMETER(const RHITexture*, Name, SampledImage, Binding, SHADER_PARAMETER_SETTER_GETTER_DEFAULT(RHITexture, Name))
#define SHADER_PARAMETER_TEXTURE_1D_ARRAY(Name, Binding) SHADER_PARAMETER(const RHITexture*, Name, SampledImage, Binding, SHADER_PARAMETER_SETTER_GETTER_DEFAULT(RHITexture, Name))

#define SHADER_PARAMETER_TEXTURE_2D(Name, Binding) SHADER_PARAMETER(const RHITexture*, Name, SampledImage, Binding, SHADER_PARAMETER_SETTER_GETTER_DEFAULT(RHITexture, Name))
#define SHADER_PARAMETER_TEXTURE_2D_ARRAY(Name, Binding) SHADER_PARAMETER(const RHITexture*, Name, SampledImage, Binding, SHADER_PARAMETER_SETTER_GETTER_DEFAULT(RHITexture, Name))

#define SHADER_PARAMETER_TEXTURE_CUBE(Name, Binding) SHADER_PARAMETER(const RHITexture*, Name, SampledImage, Binding, SHADER_PARAMETER_SETTER_GETTER_DEFAULT(RHITexture, Name))
#define SHADER_PARAMETER_TEXTURE_CUBE_ARRAY(Name, Binding) SHADER_PARAMETER(const RHITexture*, Name, SampledImage, Binding, SHADER_PARAMETER_SETTER_GETTER_DEFAULT(RHITexture, Name))

#define SHADER_PARAMETER_TEXTURE_3D(Name, Binding) SHADER_PARAMETER(const RHITexture*, Name, SampledImage, Binding, SHADER_PARAMETER_SETTER_GETTER_DEFAULT(RHITexture, Name))

#define SHADER_PARAMETER_SAMPLER(Name, Binding) SHADER_PARAMETER(const RHISampler*, Name, Sampler, Binding, SHADER_PARAMETER_SETTER_GETTER_DEFAULT(RHISampler, Name))

#define SHADER_PARAMETER_STORAGE_BUFFER(Name, Binding) SHADER_PARAMETER(const RHIBuffer*, Name, StorageBuffer, Binding, SHADER_PARAMETER_SETTER_GETTER_DEFAULT(RHIBuffer, Name)))

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

struct UniformBuffer
{
};

#define BEGIN_SHADER_PARAMETER_UNIFORM_BUFFER(Name) \
	struct UniformBuffer_##Name : public UniformBuffer {

#define UNIFORM_BUFFER_PARAMETER(Type, Name) Type Name;
#define UNIFORM_BUFFER_PARAMETER2(Type, Name) \
	private: \
		Type Name; \
	public: \
		inline const Type& Get##Name() const { return Name; } \
	    inline void Set##Name(const Type& Value) { Name = Value; }

#define END_SHADER_PARAMETER_UNIFORM_BUFFER() };

BEGIN_SHADER_PARAMETER_UNIFORM_BUFFER(WVP)
	UNIFORM_BUFFER_PARAMETER(float4x4, WorldMatrix)
	UNIFORM_BUFFER_PARAMETER(float4x4, ViewMatrix)
	UNIFORM_BUFFER_PARAMETER(float4x4, ProjectionMatrix)
END_SHADER_PARAMETER_UNIFORM_BUFFER()

#define DEFINE_SHADER_PARAMETERS_GENERIC_VS \
	BEGIN_SHADER_PARAMETER(GenericVS) \
		SHADER_PARAMETER_UNIFORM_BUFFER(WVP, WVP, 0) \
	END_SHADER_PARAMETER \

#endif  // __INCLUDE_DEFINITIONS__