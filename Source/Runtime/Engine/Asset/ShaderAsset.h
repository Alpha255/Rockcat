#pragma once

#include "Core/StringUtils.h"
#include "Core/Math/Matrix.h"
#include "Engine/Asset/SerializableAsset.h"
#include "Engine/Asset/TextureAsset.h"
#include "Engine/Application/GraphicsSettings.h"
#include "Engine/RHI/RHIShader.h"

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
		, m_Binary(new uint8_t[Size]())
	{
		VERIFY(memcpy_s(m_Binary.get(), Size, Compiled, Size) == 0);
	}

	size_t GetSize() const { return m_Size; }
	const uint8_t* GetBinary() const { return m_Binary.get(); }
	bool IsValid() const { return m_Size > 0u; }

	template<class Archive>
	void load(Archive& Ar)
	{
		Ar(
			CEREAL_NVP(m_Size)
		);

		m_Binary.reset(new uint8_t[m_Size]());

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
	std::shared_ptr<uint8_t> m_Binary;
	//void SetStatus(EStatus Status) { m_Status.store(Status); }
	//bool IsCompiling() const { return m_Status.load() == EStatus::Compiling; }
	//bool IsCompiled() const { return m_Status.load() == EStatus::Compiled; }

	//std::atomic<EStatus> m_Status;
};

struct ShaderCache : public SerializableAsset<ShaderCache>
{
	template<class T>
	ShaderCache(T&& Path)
		: BaseClass(std::forward<T>(Path))
	{
	}

	static const char* GetExtension() { return ".shadercache"; }

	bool Contains(size_t Hash) const { return CompiledBinaries.find(Hash) != CompiledBinaries.cend(); }
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

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_NVP(Value)
		);
	}
};

class ShaderAsset : public Asset, public ShaderDefines
{
public:
	template<class T>
	ShaderAsset(T&& Name)
		: Asset(GetFilePath(ASSET_PATH_SHADERS, Name))
		, m_Stage(GetStage(GetPath()))
	{
		GetDefaultDefines();
	}

	ERHIShaderStage GetStage() const { return m_Stage; }

	void Compile(bool Force = false);

	const ShaderBinary* const GetBinary(ERenderHardwareInterface RHI) const { return GetCache().GetBinary(ComputeHash(), RHI); }

	void RegisterVariable(const char* Name, ShaderVariable&& Variable)
	{
		auto It = m_Variables.find(Name);
		if (It != m_Variables.end())
		{
			It->second = std::move(Variable);

			if (It->second.IsValid())
			{
				It->second.Set(It->second.Value);
			}
		}
		else
		{
			m_Variables.insert(std::make_pair(std::string(Name), std::forward<ShaderVariable>(Variable)));
		}
	}

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_BASE(ShaderDefines),
			CEREAL_NVP(m_Variables)
		);
	}

	const std::map<std::string, ShaderVariable>& GetVariables() const { return m_Variables; }
protected:
	const ShaderCache& GetCache() const
	{
		if (!m_Cache)
		{
			m_Cache = ShaderCache::Load(GetFilePath(ASSET_PATH_SHADERCACHE, GetName(), ShaderCache::GetExtension()));
		}
		if (m_Cache->IsDirty())
		{
			m_Cache->Reload();
		}
		return *m_Cache;
	}

	static ERHIShaderStage GetStage(const std::filesystem::path& Path);
	std::map<std::string, ShaderVariable>& GetVariables() { return m_Variables; }
private:
	void GetDefaultDefines();

	mutable std::shared_ptr<ShaderCache> m_Cache;
	ERHIShaderStage m_Stage;

	std::map<std::string, ShaderVariable> m_Variables;
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

