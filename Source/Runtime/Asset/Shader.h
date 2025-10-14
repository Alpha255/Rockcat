#pragma once

#include "Core/Math/Matrix.h"
#include "Asset/SerializableAsset.h"
#include "RHI/RHITexture.h"
#include "RHI/RHIShader.h"
#include "RHI/RHIBuffer.h"
#include "Rendering/RenderSettings.h"
#include "Paths.h"

using ShaderBlob = DataBlock;

struct ShaderVariable
{
	using OnSetFunc = std::function<void(const RHIResource*)>;
	using OnSetCombinedTextureSamplerFunc = std::function<void(const RHITexture*, const RHISampler*)>;

	struct CombinedTextureSampler
	{
		const RHITexture* Texture = nullptr;
		const RHISampler* Sampler = nullptr;
	};

	ERHIShaderStage Stage = ERHIShaderStage::Num;
	ERHIResourceType Type = ERHIResourceType::Unknown;
	uint32_t Binding = ~0u;

	std::string_view Name;

	union
	{
		const RHIBuffer* Buffer;
		const RHITexture* Texture;
		const RHISampler* Sampler;
		CombinedTextureSampler CombinedTextureSampler;
	} Resource;

	mutable OnSetFunc OnSet;
	mutable OnSetCombinedTextureSamplerFunc OnSetCombinedTextureSampler;
	
	inline void BindOnSet(OnSetFunc&& Func) const
	{
		OnSet = std::move(Func);
	}

	inline void BindOnSetCombinedTextureSampler(OnSetCombinedTextureSamplerFunc&& Func) const
	{
		OnSetCombinedTextureSampler = std::move(Func);
	}

	inline const RHIBuffer* GetBuffer() const
	{
		assert(Type == ERHIResourceType::UniformBuffer || Type == ERHIResourceType::StorageBuffer || Type == ERHIResourceType::UniformBufferDynamic || Type == ERHIResourceType::StorageBufferDynamic);
		return Resource.Buffer;
	}
	inline void SetBuffer(const RHIBuffer* Buffer)
	{
		assert(Type == ERHIResourceType::UniformBuffer || Type == ERHIResourceType::StorageBuffer || Type == ERHIResourceType::UniformBufferDynamic || Type == ERHIResourceType::StorageBufferDynamic);
		Resource.Buffer = Buffer;
		if (OnSet)
		{
			OnSet(Buffer);
		}
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
		if (OnSet)
		{
			OnSet(Texture);
		}
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
		if (OnSet)
		{
			OnSet(Sampler);
		}
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
private:
	std::map<std::string, std::string> m_Defines;
};

class ShaderBinary : public Serializable<ShaderBinary>
{
public:
	using BaseClass::BaseClass;

	ShaderBinary(const class Shader& InShader, ERHIDeviceType DeviceType, ShaderBlob& Blob);

	inline const ShaderBlob& GetBlob() const
	{
		std::lock_guard<std::mutex> Lock(m_BlobLock);
		return m_Blob;
	}

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_BASE(BaseClass),
			CEREAL_NVP(m_ShaderLastWriteTime),
			CEREAL_NVP(m_Blob)
		);
	}
protected:
	friend class Shader;
	friend class ShaderLibrary;

	inline void ResetBlob()
	{
		std::lock_guard<std::mutex> Lock(m_BlobLock);
		m_Blob.Reset();
	}

	inline void SetBlob(ShaderBlob& InBlob, time_t LastWriteTime)
	{
		std::lock_guard<std::mutex> Lock(m_BlobLock);
		m_Blob = std::move(InBlob);
		m_ShaderLastWriteTime = LastWriteTime;
		Save(true);
	}
private:
	static std::filesystem::path GetPath(const Shader& InShader, ERHIDeviceType DeviceType);

	time_t m_ShaderLastWriteTime = 0;
	ShaderBlob m_Blob;
	mutable std::mutex m_BlobLock;
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

	const RHIShader* GetRHI(const class RHIDevice&);

	size_t GetHash() const;
protected:
	friend class ShaderLibrary;

	uint32_t RegisterVariable(ShaderVariable&& Variable);
	inline std::vector<ShaderVariable>& GetVariables() { return m_Variables; }

	inline void SetBinary(std::shared_ptr<ShaderBinary>& InBinary)
	{
		m_CachedBinary = std::move(InBinary);
	}
	void SetBlob(ShaderBlob& Blob, ERHIDeviceType DeviceType);

	virtual const RHIShader* GetFallback(const class RHIDevice&) const;
private:
	ERHIShaderStage m_Stage;
	std::string m_Entry;

	RHIShaderPtr m_CachedRHI;
	mutable std::shared_ptr<ShaderBinary> m_CachedBinary;

	std::vector<ShaderVariable> m_Variables;
};

template<class T>
class GlobalShader : public Shader
{
public:
	GlobalShader(const char* SubPath, ERHIShaderStage Stage, const char* EntryPoint = "main")
		: Shader(Paths::ShaderPath() / SubPath, Stage, EntryPoint)
	{
		T::RegisterShaderVariables(Cast<T>(*this));
	}
protected:
};

namespace std
{
	template<>
	struct hash<ShaderDefines>
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

	template<>
	struct hash<Shader>
	{
		size_t operator()(const Shader& InShader) const
		{
			size_t Hash = std::filesystem::hash_value(InShader.GetPath());
			HashCombine(Hash, static_cast<uint8_t>(InShader.GetStage()));
			HashCombine(Hash, std::hash<ShaderDefines>()(InShader));
			HashCombine(Hash, InShader.GetLastWriteTime());
			return Hash;
		}
	};
};

