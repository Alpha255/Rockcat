#pragma once

#include "Core/Math/Matrix.h"
#include "Engine/Asset/SerializableAsset.h"
#include "Engine/Asset/TextureAsset.h"
#include "Engine/RHI/RHIShader.h"
#include "Engine/Paths.h"

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

class ShaderBinary : public SerializableAsset<ShaderBinary>
{
public:
	template<class T>
	ShaderBinary(T&& Path, size_t Size, const void* Binary)
		: BaseClass(std::forward<T>(Path))
		, m_Size(Size)
		, m_Binary(new uint8_t[Size]())
	{
		VERIFY(memcpy_s(m_Binary.get(), Size, Binary, Size) == 0);
	}

	std::time_t GetTimestamp() const { return m_Timestamp; }
	size_t GetSize() const { return m_Size; }
	const uint8_t* GetBinary() const { return m_Binary.get(); }
	bool IsValid() const { return m_Size > 0u; }

	template<class Archive>
	void load(Archive& Ar)
	{
		Ar(
			CEREAL_NVP(m_Timestamp),
			CEREAL_NVP(m_Size)
		);

		m_Binary.reset(new uint8_t[m_Size]());

		Ar.loadBinaryValue(m_Binary.get(), m_Size, "Binary");
	}

	template<class Archive>
	void save(Archive& Ar) const
	{
		Ar(
			CEREAL_NVP(m_Timestamp),
			CEREAL_NVP(m_Size)
		);

		Ar.saveBinaryValue(m_Binary.get(), m_Size, "Binary");
	}
private:
	std::time_t m_Timestamp = 0u;
	size_t m_Size = 0u;
	std::shared_ptr<uint8_t> m_Binary;
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
	enum class ECompileStatus
	{
		None,
		Compiling,
		Compiled
	};

	const std::map<std::string, ShaderVariable>& GetVariables() const { return m_Variables; }
	RHIBuffer* GetUniformBuffer(class RHIDevice& Device);
	virtual void SetupViewParams(const class SceneView&) {}
	virtual void SetupMaterialProperty(const struct MaterialProperty&) {}
	
	virtual const ShaderMetaData& GetMetaData() const = 0;
	virtual const std::filesystem::path& GetSourceFilePath() const = 0;
	virtual std::filesystem::path GetName() const = 0;
	virtual const char* const GetEntryPoint() const = 0;
	virtual ERHIShaderStage GetStage() const = 0;

	size_t ComputeHash() const override
	{
		return ::ComputeHash(std::filesystem::hash_value(GetSourceFilePath()), ShaderDefines::ComputeHash());
	}
protected:
	void RegisterVariable(const char* Name, ShaderVariable&& Variable);
private:
	size_t ComputeUniformBufferSize();

	std::map<std::string, ShaderVariable> m_Variables;
	RHIBufferPtr m_UniformBuffer;
	std::atomic<ECompileStatus> m_CompileStatus;
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
	const char* const GetEntryPoint() const override final { return s_MetaData.GetEntryPoint(); }
	std::filesystem::path GetName() const override final { return s_MetaData.GetName(); }
protected:
	static ShaderMetaData s_MetaData;
};

#define DEFINITION_SHADER_METADATA(ShaderClass, SourceFile, Entry, Stage) ShaderMetaData GlobalShader<ShaderClass>::s_MetaData(SourceFile, Entry, Stage);

