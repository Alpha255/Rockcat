#ifndef __GLOBAL_SHADER_DEFINITIONS__
#define __GLOBAL_SHADER_DEFINITIONS__

#ifdef __cplusplus

#include "Runtime/Core/Math/Matrix.h"
using float4 = Math::Vector4;
using float3 = Math::Vector3;
using float2 = Math::Vector2;
using float4x4 = Math::Matrix;
using uint = uint32_t;

#define DECLARE_SHADER_PARAMETERS_BEGIN(ShaderType) \
	using ThisShader = ShaderType; \
	struct alignas(16) ShaderParameters { \
	private: \
	struct FirstVariableID{}; \
	using ThisShaderParametersStruct = ShaderParameters; \
	typedef void* FuncPtr; \
	typedef FuncPtr(*AddShaderVariableFunc)(FirstVariableID, MaterialAsset&, ThisShader&); \
	static FuncPtr AddShaderVariable(FirstVariableID, MaterialAsset&, ThisShader&) { return nullptr; } \
	typedef FirstVariableID

#define DECLARE_SHADER_VARIABLE(BaseType, VariableName, VariableType) \
	ThisVariableID##VariableName; \
	private: \
		BaseType VariableName; \
	public: \
		BaseType Get##VariableName() const { return VariableName; } \
		void Set##VariableName(const BaseType& Value) { VariableName = Value; } \
	private: \
		struct NextVariableID_##VariableName {}; \
		static FuncPtr AddShaderVariable(NextVariableID_##VariableName, MaterialAsset& Owner, ThisShader& Shader) \
		{ \
			MaterialProperty Property{ EShaderVariableType::VariableType, offsetof(ThisShaderParametersStruct, VariableName), sizeof(BaseType) }; \
			Property.Setter = [&Shader](const ShaderVariant& Variant) { Shader.Parameters.##VariableName=std::get<BaseType>(Variant);}; \
			Property.Getter = [&Shader](){ return ShaderVariant(Shader.Parameters.##VariableName); }; \
			Owner.RegisterProperty(#VariableName, std::move(Property)); \
			FuncPtr(*PreFunc)(ThisVariableID##VariableName, MaterialAsset&, ThisShader&); \
			PreFunc = AddShaderVariable; \
			return reinterpret_cast<FuncPtr>(PreFunc); \
		} \
		typedef NextVariableID_##VariableName

#define DECLARE_SHADER_PARAMETERS_END \
		LastVariableID; \
	private: \
		static void AddShaderVariables(MaterialAsset& Owner, ThisShader& Shader) { \
			FuncPtr(*LastFunc)(LastVariableID, MaterialAsset& Owner, ThisShader& Shader); \
			LastFunc = AddShaderVariable; \
			FuncPtr Func = reinterpret_cast<FuncPtr>(LastFunc); \
			do \
			{ \
				Func = reinterpret_cast<AddShaderVariableFunc>(Func)(FirstVariableID(), Owner, Shader); \
			} while(Func); } \
	}; \
	ShaderParameters Parameters;

#define DECLARE_UNIFORM_BUFFER_BEGIN
#define DECLARE_UNIFORM_BUFFER_END

#define DECLARE_UNIFORM_SHADER_VARIABLE(BaseType, VariableName) DECLARE_SHADER_VARIABLE(BaseType, VariableName, Uniform)

#else
#define DECLARE_UNIFORM_BUFFER_BEGIN struct UniformBuffer {
#define DECLARE_UNIFORM_BUFFER_END };

#define DECLARE_UNIFORM_SHADER_VARIABLE(BaseType, VariableName) BaseType VariableName;
#endif

#endif // __GLOBAL_SHADER_DEFINITIONS__


