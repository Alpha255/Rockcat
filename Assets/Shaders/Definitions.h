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

template<class T>
class UniformBuffer
{
public:
	RHIBuffer* GetRHI(RHIDevice&) const { return m_Buffer.get(); }
private:
	RHIBufferPtr m_Buffer;
};

#define BEGIN_SHADER_VARIABLE(Owner) \
protected: \
	using OwnerClass = Owner; \
private: \
	struct FirstVariableID{}; \
	typedef void* FuncPtr; \
	typedef FuncPtr(*RegisterShaderVariableFunc)(FirstVariableID, OwnerClass&); \
	static FuncPtr RegisterShaderVariable(FirstVariableID, OwnerClass&) { return nullptr; } \
	typedef FirstVariableID

#define SHADER_VARIABLE(Name, ResType, Binding, SetterGetter) \
	VariableID##Name; \
	private: \
		struct Name##Index { inline static uint32_t Index = 0; }; \
	public: \
		SetterGetter \
	private: \
		struct NextVariableID_##Name{}; \
		static FuncPtr RegisterShaderVariable(NextVariableID_##Name, OwnerClass& Owner) \
		{ \
			ShaderVariable Variable{ Owner.GetStage(), ERHIResourceType::ResType, Binding, ~0u, #Name }; \
			Name##Index::Index = Owner.RegisterVariable(std::move(Variable)); \
			FuncPtr(*LastShaderVariableRegisterFunc)(VariableID##Name, OwnerClass&); \
			LastShaderVariableRegisterFunc = RegisterShaderVariable; \
			return reinterpret_cast<FuncPtr>(LastShaderVariableRegisterFunc); \
		} \
		typedef NextVariableID_##Name

#define SHADER_VARIABLE_UNIFORM_BUFFER_IMPL(Type, Name, ResType, Binding, SetterGetter) \
	VariableID##Name; \
	private: \
		Type Name; \
		struct Name##Index { inline static uint32_t Index = 0; }; \
	public: \
		SetterGetter \
	private: \
		struct NextVariableID_##Name{}; \
		static FuncPtr RegisterShaderVariable(NextVariableID_##Name, OwnerClass& Owner) \
		{ \
			ShaderVariable Variable{ Owner.GetStage(), ERHIResourceType::ResType, Binding, ~0u, "UniformBuffer_" #Name }; \
			Name##Index::Index = Owner.RegisterVariable(std::move(Variable)); \
			FuncPtr(*LastShaderVariableRegisterFunc)(VariableID##Name, OwnerClass&); \
			LastShaderVariableRegisterFunc = RegisterShaderVariable; \
			return reinterpret_cast<FuncPtr>(LastShaderVariableRegisterFunc); \
		} \
		typedef NextVariableID_##Name

#define END_SHADER_VARIABLE \
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

#define SHADER_VARIABLE_SETTER_GETTER_BUFFER(Name) \
	inline const RHIBuffer* Get##Name() const { return GetVariables()[Name##Index::Index].GetBuffer(); } \
	inline OwnerClass& Set##Name(const RHIBuffer* Buffer) { GetVariables()[Name##Index::Index].SetBuffer(Buffer); return *this; }

#define SHADER_VARIABLE_SETTER_GETTER_TEXTURE(Name) \
	inline const RHITexture* Get##Name() const { return GetVariables()[Name##Index::Index].GetTexture(); } \
	inline OwnerClass& Set##Name(const RHITexture* Texture) { GetVariables()[Name##Index::Index].SetTexture(Texture); return *this; }

#define SHADER_VARIABLE_SETTER_GETTER_SAMPLER(Name) \
	inline const RHISampler* Get##Name() const { return GetVariables()[Name##Index::Index].Resource.Sampler; } \
	inline OwnerClass& Set##Name(const RHISampler* Sampler) { GetVariables()[Name##Index::Index].SetSampler(Sampler); return *this; }

#define SHADER_VARIABLE_GETTER_UNIFORM_BUFFER(Name) \
	inline UniformBuffer_##Name& GetUniformBuffer() { return Name; } \
	inline const RHIBuffer* Get##Name(RHIDevice& Device) const { return Name.GetRHI(Device); }

#define SHADER_VARIABLE_UNIFORM_BUFFER(Name, Binding) SHADER_VARIABLE_UNIFORM_BUFFER_IMPL(UniformBuffer_##Name, Name, UniformBuffer, Binding, SHADER_VARIABLE_GETTER_UNIFORM_BUFFER(Name))

#define SHADER_VARIABLE_TEXTURE_1D(Name, Binding) SHADER_VARIABLE(Name, SampledImage, Binding, SHADER_VARIABLE_SETTER_GETTER_TEXTURE(Name))
#define SHADER_VARIABLE_TEXTURE_1D_ARRAY(Name, Binding) SHADER_VARIABLE(Name, SampledImage, Binding, SHADER_VARIABLE_SETTER_GETTER_TEXTURE(Name))

#define SHADER_VARIABLE_TEXTURE_2D(Name, Binding) SHADER_VARIABLE(Name, SampledImage, Binding, SHADER_VARIABLE_SETTER_GETTER_TEXTURE(Name))
#define SHADER_VARIABLE_TEXTURE_2D_ARRAY(Name, Binding) SHADER_VARIABLE(Name, SampledImage, Binding, SHADER_VARIABLE_SETTER_GETTER_TEXTURE(Name))

#define SHADER_VARIABLE_TEXTURE_CUBE(Name, Binding) SHADER_VARIABLE(Name, SampledImage, Binding, SHADER_VARIABLE_SETTER_GETTER_TEXTURE(Name))
#define SHADER_VARIABLE_TEXTURE_CUBE_ARRAY(Name, Binding) SHADER_VARIABLE( Name, SampledImage, Binding, SHADER_VARIABLE_SETTER_GETTER_TEXTURE(Name))

#define SHADER_VARIABLE_TEXTURE_3D(Name, Binding) SHADER_VARIABLE(Name, SampledImage, Binding, SHADER_VARIABLE_SETTER_GETTER_TEXTURE(Name))

#define SHADER_VARIABLE_SAMPLER(Name, Binding) SHADER_VARIABLE(Name, Sampler, Binding, SHADER_VARIABLE_SETTER_GETTER_SAMPLER(Name))

#define SHADER_VARIABLE_STORAGE_BUFFER(Name, Binding) SHADER_VARIABLE(Name, StorageBuffer, Binding, SHADER_VARIABLE_SETTER_GETTER_BUFFER(Name)))

#define BEGIN_SHADER_VARIABLE_UNIFORM_BUFFER(Name) \
	struct UniformBuffer_##Name : public UniformBuffer<UniformBuffer_##Name> {

#define UNIFORM_BUFFER_VARIABLE(Type, Name) Type Name;

#define END_SHADER_VARIABLE_UNIFORM_BUFFER() };

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

#define BEGIN_SHADER_VARIABLE(ShaderType)

#define END_SHADER_VARIABLE

#define BEGIN_SHADER_VARIABLE_UNIFORM_BUFFER(Name) \
	struct UniformBuffer_##Name {

#define END_SHADER_VARIABLE_UNIFORM_BUFFER() };

#define UNIFORM_BUFFER_VARIABLE(Type, Name) Type Name;

#define SHADER_VARIABLE_UNIFORM_BUFFER(Name, Binding) cbuffer cbuffer_##Name : register(b##Binding) { UniformBuffer_##Name Name; };

#define SHADER_VARIABLE_TEXTURE_1D(Name, Binding) Texture1D Name : register(t##Binding);
#define SHADER_VARIABLE_TEXTURE_2D(Name, Binding) Texture2D Name : register(t##Binding);
#define SHADER_VARIABLE_TEXTURE_3D(Name, Binding) Texture3D Name : register(t##Binding);
#define SHADER_VARIABLE_TEXTURE_CUBE(Name, Binding) Texture2D Name : register(t##Binding);

#define SHADER_VARIABLE_SAMPLER(Name, Binding) SamplerState Name##_Sampler : register(s##Binding);

#define SHADER_VARIABLE_STORAGE_BUFFER(Name, Binding) RWBuffer Name : register(u##Binding);

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

BEGIN_SHADER_VARIABLE_UNIFORM_BUFFER(WVP)
	UNIFORM_BUFFER_VARIABLE(float4x4, WorldMatrix)
	UNIFORM_BUFFER_VARIABLE(float4x4, ViewMatrix)
	UNIFORM_BUFFER_VARIABLE(float4x4, ProjectionMatrix)
END_SHADER_VARIABLE_UNIFORM_BUFFER()

BEGIN_SHADER_VARIABLE_UNIFORM_BUFFER(MaterialProperty)
	UNIFORM_BUFFER_VARIABLE(float, AlphaCutoff)
END_SHADER_VARIABLE_UNIFORM_BUFFER()

#define DEFINE_SHADER_VARIABLES_GENERIC_VS \
	BEGIN_SHADER_VARIABLE(GenericVS) \
		SHADER_VARIABLE_UNIFORM_BUFFER(WVP, 0) \
	END_SHADER_VARIABLE \

#define DEFINE_SHADER_VARIABLES_DEPTH_ONLY_FS \
	BEGIN_SHADER_VARIABLE(DepthOnlyFS) \
		SHADER_VARIABLE_UNIFORM_BUFFER(MaterialProperty, 1) \
		SHADER_VARIABLE_TEXTURE_2D(BaseColorMap, 2) \
	END_SHADER_VARIABLE \

#endif  // __INCLUDE_DEFINITIONS__