#if 0
#include "Runtime/Asset/Material.h"
#include "Runtime/Asset/AssetDatabase.h"

#define STANDARD_PBR_VERTEX_SHADER     "StandardPBR.vert"
#define STANDARD_PBR_FRAGMENT_SHADER   "StandardPBR.frag"

#define BLINN_PHONG_VERTEX_SHADER       "BlinnPhong.vert"
#define BLINN_PHONG_PBR_FRAGMENT_SHADER "BlinnPhong.frag"

std::shared_ptr<ImageAsset> Material::DefaultImage;

Material::Material(const char8_t* Path)
	: Serializeable(IAsset::CatPath(ASSET_PATH_MATERIALS, Path))
	, m_DebugName(File::NameWithoutExtension(Path))
{
	for (uint32_t Stage = 0u; Stage < RHI::EShaderStage::ShaderStageCount; ++Stage)
	{
		m_PermutationBindings[Stage] = std::make_shared<ShaderPermutationBinding>();
	}

	if (!DefaultImage)
	{
		AssetDatabase::Get().GetOrReimport<ImageAsset>("white.png", [](std::shared_ptr<ImageAsset> Image) {
			DefaultImage = Image;
		});
	}
}

void Material::OnLoadCompletion()
{
	for (auto Stage = RHI::EShaderStage::Vertex; Stage < RHI::EShaderStage::ShaderStageCount; Stage = static_cast<RHI::EShaderStage>(Stage + 1))
	{
		if (m_Shaders[Stage])
		{
			SetShader(Stage, std::string(std::move(m_Shaders[Stage]->Path)).c_str(), false);
		}
	}

	for (auto Stage = EImageType::AlbedoOrDiffuse; Stage < EImageType::Unknown; Stage = static_cast<EImageType>(Stage + 1))
	{
		if (m_Images[Stage])
		{
			SetImage(Stage, std::string(std::move(m_Images[Stage]->Path)).c_str(), false);
		}
	}
}

void Material::SetShader(RHI::EShaderStage Stage, const char8_t* Path, bool8_t MarkDirty)
{
	assert(Stage < RHI::EShaderStage::ShaderStageCount && Path);

	if (!m_Shaders[Stage] || m_Shaders[Stage]->Path != Path)
	{
		AssetDatabase::Get().GetOrReimport<ShaderAsset>(Path, [this, Stage, MarkDirty](std::shared_ptr<ShaderAsset> Asset) {
			m_Shaders[Stage] = Asset;
			m_PipelineShaders.Set(Stage, Asset->Object->Get(m_PermutationBindings[Stage]->Permutation()));

			if (MarkDirty)
			{
				SetDirty();
			}
		});
	}
}

void Material::SetImage(EImageType Type, const char8_t* Path, bool8_t MarkDirty)
{
	assert(Type < EImageType::Unknown && Path);

	if (!m_Images[Type] || m_Images[Type]->Path != Path)
	{
		AssetDatabase::Get().GetOrReimport<ImageAsset>(Path, [this, Type, MarkDirty](std::shared_ptr<ImageAsset> Asset) {
			m_Images[Type] = Asset;
			
			if (MarkDirty)
			{
				SetDirty();
			}
		});
	}
}

void Material::SetShadingMode(EShadingMode Mode)
{
	switch (Mode)
	{
	case EShadingMode::BlinnPhong:
		SetShader(RHI::EShaderStage::Vertex, BLINN_PHONG_VERTEX_SHADER);
		SetShader(RHI::EShaderStage::Fragment, BLINN_PHONG_PBR_FRAGMENT_SHADER);
		break;
	case EShadingMode::PhysicallyBasedRendering:
		SetShader(RHI::EShaderStage::Vertex, STANDARD_PBR_VERTEX_SHADER);
		SetShader(RHI::EShaderStage::Fragment, STANDARD_PBR_FRAGMENT_SHADER);
		break;
	case EShadingMode::Toon:
		assert(0);
		break;
	}
}
#endif