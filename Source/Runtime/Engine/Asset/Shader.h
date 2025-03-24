#pragma once

#include "Core/Math/Matrix.h"
#include "Engine/Asset/SerializableAsset.h"
#include "Engine/Asset/TextureAsset.h"
#include "Engine/RHI/RHIShader.h"
#include "Engine/Paths.h"
#include "Engine/Application/GraphicsSettings.h"

class ShaderDefines
{
public:
	void SetDefine(const char* Name, const char* Value) { m_Defines[Name] = Value; }
	void SetDefine(const char* Name, const std::string& Value) { m_Defines[Name] = Value; }
	void SetDefine(const std::string& Name, const std::string& Value) { m_Defines[Name] = Value; }
	void SetDefine(const std::string& Name, const char* Value) { m_Defines[Name] = Value; }

	template<class T>
	void SetDefine(const char* Name, T Value) { m_Defines[Name] = (std::stringstream() << Value).str(); }

	template<class T>
	void SetDefine(const std::string& Name, T Value) { m_Defines[Name] = (std::stringstream() << Value).str(); }

	void Merge(ShaderDefines&& Other) { m_Defines.merge(std::move(Other.m_Defines)); }

	void Merge(const ShaderDefines& Other)
	{
		for (const auto& [Name, Value] : Other.m_Defines)
		{
			SetDefine(Name, Value);
		}
	}

	const std::map<std::string, std::string>& GetDefines() const { return m_Defines; }

	virtual size_t ComputeHash() const
	{
		size_t Hash = 0u;
		for (const auto& [Name, Value] : m_Defines)
		{
			HashCombine(Hash, Name);
			HashCombine(Hash, Value);
		}
		return Hash;
	}

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_NVP(m_Defines)
		);
	}
private:
	std::map<std::string, std::string> m_Defines;
};

using ShaderBlob = DataBlock;

class ShaderBinary : public SerializableAsset<ShaderBinary>
{
public:
	using BaseClass::BaseClass;

	ShaderBinary(const std::string& ShaderName, ERHIBackend Backend, ERHIShaderStage Stage, std::time_t Timestamp, size_t Hash, ShaderBlob&& Blob);

	std::time_t GetTimestamp() const { return m_Timestamp; }
	size_t GetHash() const { return m_Hash; }
	size_t GetSize() const { return m_Blob.Size; }
	ERHIBackend GetBackend() const { return m_Backend; }
	ERHIShaderStage GetStage() const { return m_Stage; }
	const std::byte* GetData() const { return m_Blob.Data.get(); }
	bool IsValid() const { return m_Blob.IsValid(); }

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_NVP(m_Timestamp),
			CEREAL_NVP(m_Hash),
			CEREAL_NVP(m_Backend),
			CEREAL_NVP(m_Blob)
		);
	}
private:
	std::time_t m_Timestamp = 0u;
	size_t m_Hash = 0u;
	ERHIBackend m_Backend = ERHIBackend::Num;
	ERHIShaderStage m_Stage = ERHIShaderStage::Num;
	ShaderBlob m_Blob;
};

struct ShaderVariable
{
	using Variant = std::variant<
		std::monostate,
		float,
		int32_t,
		uint32_t,
		Math::Vector2,
		Math::Vector3,
		Math::Vector4,
		Math::Matrix,
		std::shared_ptr<TextureAsset>>;

	using Setter = std::function<void(const Variant&)>;
	using Getter = std::function<Variant(void)>;

	ERHIResourceType Type = ERHIResourceType::Unknown;
	uint32_t Binding = ~0u;
	size_t Offset = 0u;
	size_t Stride = 0u;

	Setter Set;
	Getter Get;

	Variant Value;

	bool IsValid() const { return Value.index() != 0u; }
};

class ShaderMetaData : public Asset
{
public:
	ShaderMetaData(const char* const SubPath, const char* const Entry, ERHIShaderStage Stage)
		: Asset(Paths::ShaderPath() / SubPath)
		, m_Entry(Entry)
		, m_Stage(Stage)
	{
	}

	const char* const GetEntryPoint() const { return m_Entry; }
	ERHIShaderStage GetStage() const { return m_Stage; }
private:
	const char* const m_Entry;
	ERHIShaderStage m_Stage;
};

class Shader : public ShaderDefines
{
public:
	const std::map<std::string, ShaderVariable>& GetVariables() const { return m_Variables; }
	RHIBuffer* GetUniformBuffer(class RHIDevice& Device);

	virtual void SetupViewParams(const class SceneView&) {}
	virtual void SetupMaterialProperty(const struct MaterialProperty&) {}
	
	virtual const ShaderMetaData& GetMetaData() const = 0;
	virtual const std::filesystem::path& GetSourceFilePath() const = 0;
	virtual time_t GetTimestamp() const = 0;
	virtual std::filesystem::path GetName() const = 0;
	virtual const char* const GetEntryPoint() const = 0;
	virtual ERHIShaderStage GetStage() const = 0;

	const RHIShader* TryGetRHI(ERHIBackend Backend) const;

	size_t ComputeHash() const override { return ::ComputeHash(ComputeBaseHash(), GetTimestamp()); }
	size_t ComputeBaseHash() const { return ::ComputeHash(std::filesystem::hash_value(GetSourceFilePath()), ShaderDefines::ComputeHash()); }
protected:
	friend struct ShaderCompileTask;
	friend class ShaderLibrary;

	void RegisterVariable(const char* Name, ShaderVariable&& Variable);
	virtual ShaderMetaData& GetMetaData() = 0;
	virtual const RHIShader* GetRHIFallback() const;
private:
	size_t ComputeUniformBufferSize();

	std::map<std::string, ShaderVariable> m_Variables;
	RHIBufferPtr m_UniformBuffer;
};

template<class T>
class GlobalShader : public Shader
{
public:
	GlobalShader()
	{
		T::RegisterShaderVariables(Cast<T>(*this));
	}

	ERHIShaderStage GetStage() const override final { return s_MetaData.GetStage(); }
	const ShaderMetaData& GetMetaData() const override final { return s_MetaData; }
	const std::filesystem::path& GetSourceFilePath() const override final { return s_MetaData.GetPath(); }
	time_t GetTimestamp() const override final { return s_MetaData.GetLastWriteTime(GetSourceFilePath()); }
	const char* const GetEntryPoint() const override final { return s_MetaData.GetEntryPoint(); }
	std::filesystem::path GetName() const override final { return s_MetaData.GetStem(); }
protected:
	ShaderMetaData& GetMetaData() override final { return s_MetaData; }

	static ShaderMetaData s_MetaData;
};

#define DEFINITION_SHADER_METADATA(ShaderClass, SourceFile, Entry, Stage) ShaderMetaData GlobalShader<ShaderClass>::s_MetaData(SourceFile, Entry, Stage);

