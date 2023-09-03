#pragma once

#include "Runtime/Core/StringUtils.h"
#include "Runtime/Engine/Asset/SerializableAsset.h"
#include "Runtime/Engine/Application/GraphicsSettings.h"

class ShaderDefines
{
public:
	void SetDefine(const char8_t* Name, const char8_t* Value) { m_Defines[Name] = Value; }
	void SetDefine(const char8_t* Name, const std::string& Value) { m_Defines[Name] = Value; }
	void SetDefine(const std::string& Name, const std::string& Value) { m_Defines[Name] = Value; }
	void SetDefine(const std::string& Name, const char8_t* Value) { m_Defines[Name] = Value; }

	template<class T>
	void SetDefine(const char8_t* Name, T Value) { m_Defines[Name] = (std::stringstream() << Value).str(); }

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

class ShaderBinary : public MemoryBlock
{
public:
	enum class EStatus
	{
		None,
		Compiling,
		Compiled
	};

	using MemoryBlock::MemoryBlock;

	size_t GetSize() const { return SizeInBytes; }
	const byte8_t* GetBinary() const { return RawData.get(); }
	bool8_t IsValid() const { return SizeInBytes > 0u; }

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_BASE(MemoryBlock)
		);
	}
private:
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

	virtual const char8_t* GetExtension() const { return Asset::GetPrefabricateAssetExtension(Asset::EPrefabAssetType::ShaderCache); }

	bool8_t Contains(size_t Hash) const { return CompiledBinaries.find(Hash) != CompiledBinaries.cend(); }

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_NVP(CompiledBinaries)
		);
	}

	std::unordered_map<size_t, std::array<ShaderBinary, (size_t)ERenderHardwareInterface::Null>> CompiledBinaries;
};

class ShaderAsset : public Asset, public ShaderDefines
{
public:
	template<class StringType>
	ShaderAsset(StringType&& ShaderName)
		: Asset(std::move(std::filesystem::path(ASSET_PATH_SHADERS) / std::filesystem::path(std::forward<StringType>(ShaderName))))
	{
		GetDefaultDefines();
	}

	void Compile(bool8_t Force = false);

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_BASE(ShaderDefines)
		);
	}
protected:
	ShaderCache& GetCache()
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
private:
	void GetDefaultDefines();

	std::filesystem::path GetShaderCachePath() const
	{
		auto Path = std::filesystem::path(ASSET_PATH_SHADERCACHE) / GetPath().filename();
		Path += Asset::GetPrefabricateAssetExtension(Asset::EPrefabAssetType::ShaderCache);
		return Path;
	}

	std::shared_ptr<ShaderCache> m_Cache;
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

