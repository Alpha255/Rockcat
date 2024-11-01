#pragma once

#include "Core/Math/Matrix.h"
#include "Engine/Asset/SerializableAsset.h"
#include "Engine/Asset/TextureAsset.h"
#include "Engine/Asset/ShaderAsset.h"
#include "Runtime/Engine/RHI/RHIRenderStates.h"

enum class EShadingMode
{
	Unlit,
	BlinnPhong,
	StandardPBR,
	Toon
};

class MaterialAsset : public SerializableAsset<MaterialAsset>
{
public:
protected:
	template<class T>
	MaterialAsset(T&& Name)
		: BaseClass(GetFilePath(ASSET_PATH_MATERIALS, Name, GetExtension()))
	{
	}
public:
	virtual ~MaterialAsset() = default;

	virtual const ShaderAsset& GetVertexShader() const = 0;
	virtual const ShaderAsset& GetFragmentShader() const = 0;

	static const char* GetExtension() { return ".material"; }

	EShadingMode GetShadingMode() const { return m_ShadingMode; }

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_BASE(BaseClass),
			CEREAL_NVP(m_ShadingMode)
		);
	}
protected:
	friend class AssimpSceneImporter;

	void SetShadingMode(EShadingMode ShadingMode) { m_ShadingMode = ShadingMode; }
	EShadingMode m_ShadingMode = EShadingMode::Unlit;
};

using MaterialID = ObjectID<MaterialAsset, uint32_t>;

template<class VertexShader, class FragmentShader, class T>
class Material : public MaterialAsset, public VertexShader, public FragmentShader
{
protected:
	template<class T>
	Material(MaterialID ID, EShadingMode ShadingMode, T&& Name)
		: MaterialAsset(Name)
		, m_ID(ID)
	{
		SetShadingMode(ShadingMode);

		VertexShader::RegisterShaderVariables(*this);
		FragmentShader::RegisterShaderVariables(*this);
	}
public:
	using MaterialType = T;
	using MaterialAsset::MaterialAsset;

	const MaterialID& GetID() const { return m_ID; }
	const VertexShader& GetVertexShader() const override final { return *this; }
	const FragmentShader& GetFragmentShader() const override final { return *this; }

	template<class T>
	void SetName(T&& Name) { SetPath(GetFilePath(ASSET_PATH_MATERIALS, Name, GetExtension())); }

	ERHICullMode GetCullMode() const { return m_CullMode; }
	void SetCullMode(ERHICullMode CullMode) { m_CullMode = CullMode; }

	bool IsTwoSided() const { return m_TwoSided; }
	void SetTwoSided(bool TwoSided) { m_TwoSided = TwoSided; }

	//bool IsReady() const override final { return Asset::IsReady() && VS::IsReady() && FS::IsReady(); }
	//void Compile() override final { VS::Compile(); FS::Compile(); }

	virtual ~Material()
	{
		SerializeShaderVariables();
		Save<T>(true);
	}

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_BASE(MaterialAsset),
			CEREAL_BASE(VertexShader),
			CEREAL_BASE(FragmentShader),
			CEREAL_NVP(m_CullMode),
			CEREAL_NVP(m_TwoSided)
		);
	}
protected:
	void SerializeShaderVariables()
	{
		for (auto& Variable : VertexShader::GetVariables())
		{
			Variable.second.Value = Variable.second.Get();
		}
		for (auto& Variable : FragmentShader::GetVariables())
		{
			Variable.second.Value = Variable.second.Get();
		}
	}

private:
	MaterialID m_ID;
	ERHICullMode m_CullMode = ERHICullMode::BackFace;
	bool m_TwoSided = false;
};
