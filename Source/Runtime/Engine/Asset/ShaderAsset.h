#pragma once

#include "Runtime/Core/StringUtils.h"
#include "Runtime/Engine/Asset/SerializableAsset.h"

class ShaderDefinitions
{
public:
	void SetDefine(const char8_t* Name, const char8_t* Value)
	{
		m_Definitions[Name] = Value;
	}

	void SetDefine(const char8_t* Name, const std::string& Value)
	{
		m_Definitions[Name] = Value;
	}

	template<class T>
	void SetDefine(const char8_t* Name, T Value)
	{
		m_Definitions[Name] = (std::stringstream() << Value).str();
	}

	void Merge(ShaderDefinitions&& Other)
	{
		m_Definitions.merge(Other.m_Definitions);
	}

	void Merge(const ShaderDefinitions& Other)
	{
		for each (const auto& NameValue in Other.m_Definitions)
		{
			SetDefine(NameValue.first.c_str(), NameValue.second);
		}
	}

	const std::map<std::string, std::string>& GetDefinitions() const
	{
		return m_Definitions;
	}

	std::string GetPermutationString() const
	{
		std::string PermutationString;
		for each (const auto& NameValue in m_Definitions)
		{
			PermutationString += NameValue.first + "=" + NameValue.second + "\n";
		}
		return PermutationString;
	}
private:
	std::map<std::string, std::string> m_Definitions;
};

class ShaderBinary : private AssetData
{
public:
	ShaderBinary(size_t DataSize, const byte8_t* const Data)
	{
		SizeInBytes = DataSize;
		RawData.reset(new byte8_t[DataSize]());
		VERIFY(memcpy_s(RawData.get(), DataSize, Data, DataSize) == 0);
	}

	size_t GetSize() const
	{
		return SizeInBytes;
	}

	const byte8_t* GetBinary() const
	{
		return RawData.get();
	}

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
	ShaderCache(const char8_t* ShaderName)
		: SerializableAsset(ConvertToShaderCachePath(ShaderName))
	{
	}

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_BASE(ParentClass)
		);
	}
private:
	std::string ConvertToShaderCachePath(const char8_t* ShaderName);
	std::unordered_map<std::string, ShaderBinary> m_CachedShaderBinaries;
};

class ShaderAsset : public Asset
{
public:
	const char8_t* const GetSourceCode() const
	{
		return GetData()->RawData.get();
	}

	const ShaderBinary* const GetShaderBinary(const ShaderDefinitions&) const
	{
		return nullptr;
	}
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
	//template<> struct LoadAndConstruct<ShaderBinary>
	//{
	//	template<class Archive>
	//	static void load_and_construct(Archive& Ar, cereal::construct<ShaderBinary>& Construct)
	//	{
	//		size_t DataSize = 0u;
	//		const byte8_t* const Data = nullptr;
	//		Ar(DataSize);
	//		Ar(Data);
	//		Construct(DataSize, Data);
	//	}
	//};

	template<> struct LoadAndConstruct<ShaderCache>
	{
		template<class Archive>
		static void load_and_construct(Archive& Ar, cereal::construct<ShaderCache>& Construct)
		{
			const char8_t* ShaderName = nullptr;
			Ar(ShaderName);
			Construct(ShaderName);
		}
	};
}

