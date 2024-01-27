#pragma once

#include "Runtime/Core/StringUtils.h"
#include "Runtime/Engine/Asset/SerializableAsset.h"
#include "Runtime/Engine/Application/GraphicsSettings.h"
#include "Runtime/Engine/RHI/RHIShader.h"

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

	size_t ComputeHash() const
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

class ShaderBinary
{
public:
	enum class EStatus
	{
		None,
		Compiling,
		Compiled
	};

	ShaderBinary() = default;

	ShaderBinary(size_t Size, const void* Compiled)
		: m_Size(Size)
		, m_Binary(new byte8_t[Size]())
	{
		VERIFY(memcpy_s(m_Binary.get(), Size, Compiled, Size) == 0);
	}

	size_t GetSize() const { return m_Size; }
	const byte8_t* GetBinary() const { return m_Binary.get(); }
	bool8_t IsValid() const { return m_Size > 0u; }

	template<class Archive>
	void load(Archive& Ar)
	{
		Ar(
			CEREAL_NVP(m_Size)
		);

		m_Binary.reset(new byte8_t[m_Size]());

		Ar.loadBinaryValue(m_Binary.get(), m_Size, "Compiled");
	}

	template<class Archive>
	void save(Archive& Ar) const
	{
		Ar(
			CEREAL_NVP(m_Size)
		);

		Ar.saveBinaryValue(m_Binary.get(), m_Size, "Compiled");
	}
private:
	size_t m_Size = 0u;
	std::shared_ptr<byte8_t> m_Binary;
	//void SetStatus(EStatus Status) { m_Status.store(Status); }
	//bool8_t IsCompiling() const { return m_Status.load() == EStatus::Compiling; }
	//bool8_t IsCompiled() const { return m_Status.load() == EStatus::Compiled; }

	//std::atomic<EStatus> m_Status;
};

struct ShaderCache : public SerializableAsset<ShaderCache>
{
	template<class StringType>
	ShaderCache(StringType&& Path)
		: BaseClass(std::forward<StringType>(Path))
	{
	}

	virtual const char* GetExtension() const { return Asset::GetPrefabricateAssetExtension(Asset::EPrefabAssetType::ShaderCache); }

	bool8_t Contains(size_t Hash) const { return CompiledBinaries.find(Hash) != CompiledBinaries.cend(); }
	const ShaderBinary* const GetBinary(size_t Hash, ERenderHardwareInterface RHI) const
	{
		auto It = CompiledBinaries.find(Hash);
		if (It != CompiledBinaries.end())
		{
			return &It->second.at((size_t)RHI);
		}
		return nullptr;
	}

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_NVP(CompiledBinaries)
		);
	}

	std::unordered_map<size_t, std::array<ShaderBinary, (size_t)ERenderHardwareInterface::Num>> CompiledBinaries;
};

class ShaderAsset : public Asset, public ShaderDefines
{
public:
	template<class StringType>
	ShaderAsset(StringType&& ShaderName)
		: Asset(std::move(std::filesystem::path(ASSET_PATH_SHADERS) / std::filesystem::path(std::forward<StringType>(ShaderName))))
		, m_Stage(GetStage(GetPath()))
	{
		GetDefaultDefines();
	}

	ERHIShaderStage GetStage() const { return m_Stage; }

	void Compile(bool8_t Force = false);

	const ShaderBinary* const GetBinary(ERenderHardwareInterface RHI) const { return GetCache().GetBinary(ComputeHash(), RHI); }

	RHIShaderCreateInfo GetRHICreateInfo(ERenderHardwareInterface RHI) const;

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_BASE(ShaderDefines)
		);
	}
protected:
	ShaderCache& GetCache() const
	{
		if (!m_Cache)
		{
			m_Cache = ShaderCache::Load(GetShaderCachePath());
		}
		if (m_Cache->IsDirty())
		{
			m_Cache->Reload();
		}
		return *m_Cache;
	}

	static ERHIShaderStage GetStage(const std::filesystem::path& ShaderPath)
	{
		auto Extension = StringUtils::Lowercase(ShaderPath.extension().generic_string());
		if (Extension == ".vert")
		{
			return ERHIShaderStage::Vertex;
		}
		else if (Extension == ".hull")
		{
			return ERHIShaderStage::Hull;
		}
		else if (Extension == ".domain")
		{
			return ERHIShaderStage::Domain;
		}
		else if (Extension == ".geom")
		{
			return ERHIShaderStage::Geometry;
		}
		else if (Extension == ".frag")
		{
			return ERHIShaderStage::Fragment;
		}
		else if (Extension == ".comp")
		{
			return ERHIShaderStage::Compute;
		}
		return ERHIShaderStage::Num;
	}
private:
	void GetDefaultDefines();

	std::filesystem::path GetShaderCachePath() const
	{
		auto Path = std::filesystem::path(ASSET_PATH_SHADERCACHE) / GetPath().filename();
		Path += Asset::GetPrefabricateAssetExtension(Asset::EPrefabAssetType::ShaderCache);
		return Path;
	}

	mutable std::shared_ptr<ShaderCache> m_Cache;

	ERHIShaderStage m_Stage;
};

namespace cereal
{
	template<> struct LoadAndConstruct<ShaderCache>
	{
		template<class Archive>
		static void load_and_construct(Archive& Ar, cereal::construct<ShaderCache>& Construct)
		{
			std::string NullShaderName;
			Ar(NullShaderName);
			Construct(NullShaderName);
		}
	};
}

