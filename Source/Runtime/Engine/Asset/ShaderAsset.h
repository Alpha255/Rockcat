#pragma once

#include "Runtime/Core/StringUtils.h"
#include "Runtime/Engine/Asset/SerializableAsset.h"

class ShaderDefines
{
public:
	void SetDefine(const char8_t* Name, const char8_t* Value) { m_Definitions[Name] = Value; }
	void SetDefine(const char8_t* Name, const std::string& Value) { m_Definitions[Name] = Value; }

	template<class T>
	void SetDefine(const char8_t* Name, T Value) { m_Definitions[Name] = (std::stringstream() << Value).str(); }

	void Merge(ShaderDefines&& Other) { m_Definitions.merge(Other.m_Definitions); }

	void Merge(const ShaderDefines& Other)
	{
		for each (const auto& NameValue in Other.m_Definitions)
		{
			SetDefine(NameValue.first.c_str(), NameValue.second);
		}
	}

	const std::map<std::string, std::string>& GetDefinitions() const { return m_Definitions; }
private:
	std::map<std::string, std::string> m_Definitions;
};

class ShaderVariantMask : public std::bitset<sizeof(uint32_t)>
{
};

class ShaderBinary : private AssetRawData
{
public:
	ShaderBinary(size_t DataSize, const byte8_t* const InData)
	{
		SizeInBytes = DataSize;
		Data.reset(new byte8_t[DataSize]());
		VERIFY(memcpy_s(Data.get(), DataSize, InData, DataSize) == 0);
	}

	size_t GetSize() const { return SizeInBytes; }
	const byte8_t* GetBinary() const { return Data.get(); }

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_NVP(SizeInBytes),
			CEREAL_NVP(RawData)
		);
	}
};

class ShaderCache : public SerializableAsset<ShaderCache>
{
public:
	template<class StringType>
	ShaderCache(StringType&& ShaderName)
		: ParentClass(GetShaderCachePath(std::filesystem::path(std::move(ShaderName)).generic_string().c_str()))
	{
	}

	virtual const char8_t* GetExtension() const { return Asset::GetPrefabricateAssetExtension(Asset::EPrefabAssetType::ShaderCache); }

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_BASE(ParentClass)
		);
	}
private:
	std::string GetShaderCachePath(const char8_t* ShaderName);
	std::unordered_map<std::string, ShaderBinary> m_CachedShaderBinaries;
};

class ShaderAsset : public Asset, public ShaderDefines
{
public:
	template<class StringType>
	ShaderAsset(StringType&& ShaderName)
		: Asset(std::move(std::filesystem::path(ASSET_PATH_SHADERS) / std::filesystem::path(std::forward<StringType>(ShaderName))))
		, m_Cache(ShaderCache::Load(std::forward<StringType>(ShaderName)))
	{
	}

	void Compile() {}

	const char8_t* const GetSourceCode() const { return GetRawData().Data.get(); }
private:
	std::shared_ptr<ShaderCache> m_Cache;
};

struct MemoryBlock : public SerializableAsset<MemoryBlock>
{
public:
	using ParentClass::ParentClass;

	size_t SizeInBytes = 0u;
	std::shared_ptr<byte8_t> RawData;

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_BASE(ParentClass),
			CEREAL_NVP(SizeInBytes)
		);

		if (Archive::is_loading::value)
		{
			std::vector<uint64_t> Buffer;
			Ar(
				cereal::make_nvp("RawData", Buffer)
			);
			RawData.reset(new byte8_t[SizeInBytes]);
			memcpy_s(RawData.get(), SizeInBytes, Buffer.data(), SizeInBytes);
		}
		else
		{
			std::vector<uint64_t> Buffer(SizeInBytes / sizeof(uint64_t));
			memcpy_s(Buffer.data(), SizeInBytes, RawData.get(), SizeInBytes);
			Ar(
				cereal::make_nvp("RawData", Buffer)
			);
		}
	}
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

