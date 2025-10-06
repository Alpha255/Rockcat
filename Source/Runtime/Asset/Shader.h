#pragma once

#include "Core/Math/Matrix.h"
#include "Asset/SerializableAsset.h"
#include "RHI/RHITexture.h"
#include "RHI/RHIShader.h"
#include "RHI/RHIBuffer.h"
#include "Rendering/RenderSettings.h"

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
		const RHITexture*,
		const RHISampler*,
		const RHIBuffer*>;

	using Setter = std::function<void(const Variant&)>;
	using Getter = std::function<Variant(void)>;

	ERHIResourceType Type = ERHIResourceType::Unknown;
	uint32_t Binding = ~0u;
	size_t Offset = 0u;
	size_t Stride = 0u;

	Setter Set;
	Getter Get;

	Variant Value;

	inline bool IsValid() const { return Value.index() != 0u; }
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

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_BASE(BaseClass)
		);
	}
private:
	static std::filesystem::path GetUniquePath(const Shader& InShader, ERHIDeviceType DeviceType);

	DataBlock m_Blob;
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

	inline const std::map<std::string_view, ShaderVariable>& GetVariables() const { return m_Variables; }
protected:
	friend struct ShaderCompileTask;
	friend class ShaderLibrary;

	void RegisterVariable(const char* Name, ShaderVariable&& Variable);
private:
	ERHIShaderStage m_Stage;
	std::string m_Entry;

	std::map<std::string_view, ShaderVariable> m_Variables;
};

template<class T>
class GlobalShader : public Shader
{
public:
	GlobalShader()
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
			size_t Hash = std::hash<ShaderDefines>()(InShader);
			HashCombine(Hash, static_cast<uint8_t>(InShader.GetStage()));
			return Hash;
		}
	};
};

