#pragma once

#include "Core/Math/Matrix.h"
#include "Asset/SerializableAsset.h"
#include "RHI/RHITexture.h"
#include "RHI/RHIShader.h"
#include "RHI/RHIBuffer.h"
#include "Rendering/RenderSettings.h"

using ShaderBlob = DataBlock;

struct ShaderVariable
{
	struct CombinedTextureSampler
	{
		const RHITexture* Texture = nullptr;
		const RHISampler* Sampler = nullptr;
	};

	ERHIShaderStage Stage = ERHIShaderStage::Num;
	ERHIResourceType Type = ERHIResourceType::Unknown;
	uint32_t Binding = ~0u;
	uint32_t DescriptorIndex = ~0u;

	std::string_view Name;

	union
	{
		const RHIBuffer* Buffer;
		const RHITexture* Texture;
		const RHISampler* Sampler;
		CombinedTextureSampler CombinedTextureSampler;
	} Resource;

	inline const RHIBuffer* GetBuffer() const
	{
		assert(Type == ERHIResourceType::UniformBuffer || Type == ERHIResourceType::StorageBuffer || Type == ERHIResourceType::UniformBufferDynamic || Type == ERHIResourceType::StorageBufferDynamic);
		return Resource.Buffer;
	}
	inline void SetBuffer(const RHIBuffer* Buffer)
	{
		assert(Type == ERHIResourceType::UniformBuffer || Type == ERHIResourceType::StorageBuffer || Type == ERHIResourceType::UniformBufferDynamic || Type == ERHIResourceType::StorageBufferDynamic);
		Resource.Buffer = Buffer;
	}

	inline const RHITexture* GetTexture() const
	{
		assert(Type == ERHIResourceType::SampledImage || Type == ERHIResourceType::StorageImage);
		return Resource.Texture;
	}
	inline void SetTexture(const RHITexture* Texture)
	{
		assert(Type == ERHIResourceType::SampledImage || Type == ERHIResourceType::StorageImage);
		Resource.Texture = Texture;
	}

	inline const RHISampler* GetSampler() const
	{
		assert(Type == ERHIResourceType::Sampler);
		return Resource.Sampler;
	}
	inline void SetSampler(const RHISampler* Sampler)
	{
		assert(Type == ERHIResourceType::Sampler);
		Resource.Sampler = Sampler;
	}

	inline const CombinedTextureSampler& GetCombinedTextureSampler() const
	{
		assert(Type == ERHIResourceType::CombinedImageSampler);
		return Resource.CombinedTextureSampler;
	}
	inline void SetCombinedTextureSampler(const RHITexture* Texture, const RHISampler* Sampler)
	{
		assert(Type == ERHIResourceType::CombinedImageSampler);
		Resource.CombinedTextureSampler.Texture = Texture;
		Resource.CombinedTextureSampler.Sampler = Sampler;
	}
};

class ShaderDefines
{
public:
	template<class T>
	inline void SetDefine(const char* Name, T Value) { m_Defines[Name] = (std::stringstream() << Value).str(); }

	template<class T>
	inline void SetDefine(const std::string& Name, T Value) { m_Defines[Name] = (std::stringstream() << Value).str(); }

	void Merge(const ShaderDefines& Other)
	{
		for (const auto& [Name, Value] : Other.m_Defines)
		{
			SetDefine(Name, Value);
		}
	}

	const std::map<std::string, std::string>& GetDefines() const { return m_Defines; }

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

class ShaderBinary : public Serializable<ShaderBinary>
{
public:
	using BaseClass::BaseClass;

	ShaderBinary(const class Shader& InShader, ERHIDeviceType DeviceType);

	inline const ShaderBlob& GetBlob() const { return m_Blob; }

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_BASE(BaseClass)
		);
	}
private:
	static std::filesystem::path GetPath(const Shader& InShader, ERHIDeviceType DeviceType);

	ShaderBlob m_Blob;
};

class Shader : public Asset, public ShaderDefines
{
public:
	Shader(const std::filesystem::path& Path, ERHIShaderStage Stage, const char* EntryPoint)
		: Asset(Path)
		, m_Stage(Stage)
		, m_Entry(EntryPoint)
	{
	}

	Shader(std::filesystem::path&& Path, ERHIShaderStage Stage, const char* EntryPoint)
		: Asset(std::forward<std::filesystem::path>(Path))
		, m_Stage(Stage)
		, m_Entry(EntryPoint)
	{
	}

	inline ERHIShaderStage GetStage() const { return m_Stage; }
	inline const char* GetEntryPoint() const { return m_Entry.c_str(); }

	inline const std::vector<ShaderVariable>& GetVariables() const { return m_Variables; }
protected:
	friend struct ShaderCompileTask;
	friend class ShaderLibrary;

	uint32_t RegisterVariable(ShaderVariable&& Variable);
	inline std::vector<ShaderVariable>& GetVariables() { return m_Variables; }
private:
	ERHIShaderStage m_Stage;
	std::string m_Entry;

	std::vector<ShaderVariable> m_Variables;
};

template<class T>
class GlobalShader : public Shader
{
public:
	GlobalShader(const std::filesystem::path& Path, ERHIShaderStage Stage, const char* EntryPoint)
		: Shader(Path, Stage, EntryPoint)
	{
		T::RegisterShaderVariables(Cast<T>(*this));
	}

	GlobalShader(std::filesystem::path&& Path, ERHIShaderStage Stage, const char* EntryPoint)
		: Shader(std::forward<std::filesystem::path>(Path), Stage, EntryPoint)
	{
		T::RegisterShaderVariables(Cast<T>(*this));
	}
protected:
};

namespace std
{
	template<> struct hash<ShaderDefines>
	{
		size_t operator()(const ShaderDefines& Defines) const
		{
			size_t Hash = 0u;
			for (const auto& [Name, Value] : Defines.GetDefines())
			{
				HashCombine(Hash, Name);
				HashCombine(Hash, Value);
			}
			return Hash;
		}
	};

	template<> struct hash<Shader>
	{
		size_t operator()(const Shader& InShader) const
		{
			size_t Hash = std::filesystem::hash_value(InShader.GetPath());
			HashCombine(Hash, static_cast<uint8_t>(InShader.GetStage()));
			HashCombine(Hash, std::hash<ShaderDefines>()(InShader));
			return Hash;
		}
	};
};

