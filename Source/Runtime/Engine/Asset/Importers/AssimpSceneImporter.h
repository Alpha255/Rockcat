#pragma once

#include "Runtime/Engine/Asset/SceneAsset.h"
#include <Submodules/assimp/include/assimp/Importer.hpp>
#include <Submodules/assimp/include/assimp/ProgressHandler.hpp>
#include <Submodules/assimp/include/assimp/scene.h>
#include <Submodules/assimp/include/assimp/postprocess.h>
#include <Submodules/assimp/include/assimp/DefaultLogger.hpp>

class AssimpSceneImporter : public IAssetImporter
{
public:
	AssimpSceneImporter()
		: IAssetImporter({
			AssetType("Autodesk 3dsMax", ".3ds"), 
			AssetType("Blender 3D", ".blend"), 
			AssetType("OpenCOLLADA", ".dae", AssetType::EContentsType::Text),
			AssetType("Autodesk FBX", ".fbx"), 
			AssetType("Graphics Language Transmission Format", ".gltf", AssetType::EContentsType::Text),
			AssetType("Wavefront", ".obj", AssetType::EContentsType::Text),
			AssetType("Polygon File Format", ".ply", AssetType::EContentsType::Text),
			AssetType("Stereolithography", ".stl", AssetType::EContentsType::Text),
			AssetType("XFile", ".x", AssetType::EContentsType::Text)})
	{
	}

	std::shared_ptr<Asset> CreateAsset(const std::filesystem::path& AssetPath) override final { return std::make_shared<AssimpSceneAsset>(AssetPath); }

	bool8_t Reimport(Asset& InAsset) override final
	{
		auto& Scene = Cast<AssimpSceneAsset>(InAsset);

		const uint32_t PostprocessFlags = static_cast<uint32_t>(
			aiProcessPreset_TargetRealtime_MaxQuality | 
			aiProcess_MakeLeftHanded |  /// Use DirectX's left-hand coordinate system
			aiProcess_FlipUVs |
			aiProcess_FlipWindingOrder |
			aiProcess_GenBoundingBoxes);

		Assimp::Importer AssimpNativeImporter;

#if _DEBUG
		Assimp::DefaultLogger::set(new AssimpLogger());
		AssimpNativeImporter.SetProgressHandler(new AssimpProgressHandler(Scene.GetPath()));
#endif
		auto OldWorkingDirectory = PlatformMisc::GetCurrentWorkingDirectory();
		auto CurrentWorkingDirectory = (OldWorkingDirectory / Scene.GetPath()).parent_path();

		PlatformMisc::SetCurrentWorkingDirectory(CurrentWorkingDirectory);
		auto AssimpScene = AssimpNativeImporter.ReadFileFromMemory(Scene.GetRawData().Data.get(), Scene.GetRawData().SizeInBytes, PostprocessFlags);
		PlatformMisc::SetCurrentWorkingDirectory(OldWorkingDirectory);

		if (AssimpScene && AssimpScene->HasMeshes())
		{
			return true;
		}
		else
		{
			auto ErrorInfo = AssimpNativeImporter.GetErrorString();
			LOG_ERROR("AssimpSceneImporter:: Failed to load assimp scene \"{}\" : {}", Scene.GetPath().generic_string(), std::strlen(ErrorInfo) > 0u ? ErrorInfo : "Unknown exceptions");
		}

		return false;
	}
protected:
private:
	class AssimpProgressHandler : public Assimp::ProgressHandler
	{
	public:
		AssimpProgressHandler(const std::filesystem::path& AssetPath)
			: m_AssetPath(AssetPath)
		{
		}

		bool8_t Update(float32_t Percentage) override final
		{
			if (Percentage >= 1.0f)
			{
				LOG_INFO("AssimpSceneImporter: Loading model \"{}\" succeeded", m_AssetPath.generic_string());
				return true;
			}

			if (static_cast<int32_t>(Percentage * 100) % 10 == 0)
			{
				LOG_INFO("AssimpSceneImporter: Loading model: \"{}\" in progress {:.2f}%", m_AssetPath.generic_string(), Percentage * 100);
			}
			return false;
		}
	private:
		const std::filesystem::path& m_AssetPath;
	};

	class AssimpLogger : public Assimp::Logger
	{
	public:
		void OnDebug(const char* Message) override final { LOG_DEBUG("AssimpSceneImporter: {}", Message); }
		void OnVerboseDebug(const char* Message) override final { LOG_DEBUG("AssimpSceneImporter: {}", Message); }
		void OnInfo(const char* Message) override final { LOG_INFO("AssimpSceneImporter: {}", Message); }
		void OnWarn(const char* Message) override final { LOG_WARNING("AssimpSceneImporter: {}", Message); }
		void OnError(const char* Message) override final { LOG_ERROR("AssimpSceneImporter: {}", Message); }
		bool8_t attachStream(Assimp::LogStream*, uint32_t) override final { return true; }
		bool8_t detachStream(Assimp::LogStream*, uint32_t) override final { return true; }
	};
	//void ProcessNode(const aiScene* AssimpScene, const aiNode* AssimpNode, ModelAsset* Model);
	//std::pair<std::shared_ptr<Mesh>, std::shared_ptr<Material>> ProcessMesh(const aiScene* AssimpScene, const aiMesh* AssimpMesh, ModelAsset* Model);
	//std::shared_ptr<Material> ProcessMaterial(const aiScene* AssimpScene, const aiMesh* AssimpMesh, ModelAsset* Model);
};

#if 0
void AssimpImporter::ProcessNode(const aiScene* AssimpScene, const aiNode* AssimpNode, ModelAsset* Model)
{
	assert(AssimpScene && AssimpNode && Model);

	for (uint32_t MeshIndex = 0u; MeshIndex < AssimpNode->mNumMeshes; ++MeshIndex)
	{
		auto SubMesh = ProcessMesh(AssimpScene, AssimpScene->mMeshes[AssimpNode->mMeshes[MeshIndex]], Model);

		Model->Object->m_Meshes.emplace_back(MeshInstance(SubMesh.first, SubMesh.second));
		Model->Object->VertexCount += SubMesh.first->VertexCount;
		Model->Object->FaceCount += SubMesh.first->FaceCount;
		Model->Object->IndexCount += SubMesh.first->IndexCount;

		auto BoundingBox = Model->Object->BoundingBox();
		auto SubBoundingBox = SubMesh.first->BoundingBox();
		auto Min = Math::Min(BoundingBox.Min(), SubBoundingBox.Min());
		auto Max = Math::Max(BoundingBox.Max(), SubBoundingBox.Max());
		Model->Object->AxisAlignedBoundingBox = AABB(Min, Max);
	}

	for (uint32_t Index = 0u; Index < AssimpNode->mNumChildren; ++Index)
	{
		ProcessNode(AssimpScene, AssimpNode->mChildren[Index], Model);
	}
}

std::shared_ptr<Material> AssimpImporter::ProcessMaterial(const aiScene* AssimpScene, const aiMesh* AssimpMesh, ModelAsset* Model)
{
	if (AssimpScene->HasMaterials())
	{
		assert(AssimpMesh->mMaterialIndex < AssimpScene->mNumMaterials);
		auto CurrentMaterial = AssimpScene->mMaterials[AssimpMesh->mMaterialIndex];

		auto ModelName = File::NameWithoutExtension(Model->Path);

		aiString MaterialName;
		aiGetMaterialString(CurrentMaterial, AI_MATKEY_NAME, &MaterialName);

		std::string Path;
		if (MaterialName.length == 0)
		{
			Path = StringUtils::Format("%s.json", ModelName.c_str());
		}
		else
		{
			Path = StringUtils::Format("%s_%s.json", ModelName.c_str(), MaterialName.C_Str());
		}

		if (File::Exists(IAsset::CatPath(ASSET_PATH_MATERIALS, Path.c_str()).c_str()))
		{
			return Material::Load(Path.c_str());
		}
		else
		{
			auto Ret = std::make_shared<Material>(Path.c_str());

			aiShadingMode ShadingMode = aiShadingMode::aiShadingMode_Flat;
			aiColor4D Albedo{ 1.0f };
			aiColor4D Ambient{ 1.0f };
			aiColor4D Specular{ 0.0f };
			aiColor4D Emissive{ 0.0f };
			aiColor4D Transparent{ 0.0f };
			aiColor4D Reflective{ 0.0f };
			ai_real Opacity = 1.0f;
			ai_real Metallic = 0.0f;
			ai_real Roughness = 0.0f;
			ai_real Glossiness = 0.0f;
			ai_real SpecularFactor = 0.0f;
			ai_real Shiness = 0.0f;
			ai_int Twoside = 0;

			if (AI_SUCCESS == aiGetMaterialInteger(CurrentMaterial, AI_MATKEY_SHADING_MODEL, reinterpret_cast<int32_t*>(&ShadingMode)))
			{
				if (ShadingMode == aiShadingMode::aiShadingMode_PBR_BRDF)
				{
					Ret->SetShadingMode(Material::EShadingMode::PhysicallyBasedRendering);

					if (AI_SUCCESS == aiGetMaterialColor(CurrentMaterial, AI_MATKEY_BASE_COLOR, &Albedo))
					{
						Ret->SetAlbedoOrDiffuse(Color(Albedo.r, Albedo.g, Albedo.b, Albedo.a));
					}
					if (AI_SUCCESS == aiGetMaterialFloat(CurrentMaterial, AI_MATKEY_METALLIC_FACTOR, &Metallic))
					{
						Ret->SetMetallic(Metallic);
					}
					if (AI_SUCCESS == aiGetMaterialFloat(CurrentMaterial, AI_MATKEY_ROUGHNESS_FACTOR, &Roughness))
					{
						Ret->SetRoughness(Roughness);
					}
					else if (AI_SUCCESS == aiGetMaterialFloat(CurrentMaterial, AI_MATKEY_GLOSSINESS_FACTOR, &Glossiness))
					{
						Ret->SetRoughness(1.0f - Glossiness);
					}
					if (AI_SUCCESS == aiGetMaterialFloat(CurrentMaterial, AI_MATKEY_SPECULAR_FACTOR, &SpecularFactor))
					{
						Ret->SetSpecularFactor(SpecularFactor);
					}
				}
				else if (ShadingMode == aiShadingMode::aiShadingMode_Toon)
				{
					Ret->SetShadingMode(Material::EShadingMode::Toon);

					assert(0);
				}
				else
				{
					Ret->SetShadingMode(Material::EShadingMode::BlinnPhong);

					if (AI_SUCCESS == aiGetMaterialColor(CurrentMaterial, AI_MATKEY_COLOR_DIFFUSE, &Albedo))
					{
						Ret->SetAlbedoOrDiffuse(Color(Albedo.r, Albedo.g, Albedo.b, Albedo.a));
					}
				}
			}
			else
			{
				Ret->SetShadingMode(Material::EShadingMode::BlinnPhong);

				if (AI_SUCCESS == aiGetMaterialColor(CurrentMaterial, AI_MATKEY_COLOR_DIFFUSE, &Albedo))
				{
					Ret->SetAlbedoOrDiffuse(Color(Albedo.r, Albedo.g, Albedo.b, Albedo.a));
				}
			}

			if (AI_SUCCESS == aiGetMaterialColor(CurrentMaterial, AI_MATKEY_COLOR_SPECULAR, &Specular))
			{
				Ret->SetSpecular(Color(Specular.r, Specular.g, Specular.b, Specular.a));
			}
			if (AI_SUCCESS == aiGetMaterialColor(CurrentMaterial, AI_MATKEY_COLOR_EMISSIVE, &Emissive))
			{
				Ret->SetEmissive(Color(Emissive.r, Emissive.g, Emissive.b, Emissive.a));
			}
			if (AI_SUCCESS == aiGetMaterialColor(CurrentMaterial, AI_MATKEY_COLOR_TRANSPARENT, &Transparent))
			{
				Ret->SetTransparent(Color(Transparent.r, Transparent.g, Transparent.b, Transparent.a));
			}
			if (AI_SUCCESS == aiGetMaterialColor(CurrentMaterial, AI_MATKEY_COLOR_REFLECTIVE, &Reflective))
			{
				Ret->SetReflective(Color(Reflective.r, Reflective.g, Reflective.b, Reflective.a));
			}
			if (AI_SUCCESS == aiGetMaterialFloat(CurrentMaterial, AI_MATKEY_OPACITY, &Opacity))
			{
				Ret->SetOpacity(Opacity);
			}
			if (AI_SUCCESS == aiGetMaterialFloat(CurrentMaterial, AI_MATKEY_SHININESS, &Shiness))
			{
				Ret->SetShiness(Shiness);
			}
			if (AI_SUCCESS == aiGetMaterialInteger(CurrentMaterial, AI_MATKEY_TWOSIDED, &Twoside))
			{
				Ret->SetTwoSide(Twoside);
			}

			for (uint32_t ImageType = aiTextureType::aiTextureType_DIFFUSE; ImageType < aiTextureType::aiTextureType_TRANSMISSION; ++ImageType)
			{
				if (CurrentMaterial->GetTextureCount(static_cast<aiTextureType>(ImageType)) > 0u)
				{
					aiString ImageName;
					if (aiReturn_SUCCESS == CurrentMaterial->GetTexture(static_cast<aiTextureType>(ImageType), 0u, &ImageName))
					{
						std::string ImagePath = StringUtils::Format("%s\\%s", File::Directory(Model->Path).c_str(), ImageName.C_Str());
						switch (ImageType)
						{
						case aiTextureType::aiTextureType_DIFFUSE:
						case aiTextureType::aiTextureType_BASE_COLOR:
							Ret->SetImage(Material::EImageType::AlbedoOrDiffuse, ImagePath.c_str());
							break;
						case aiTextureType::aiTextureType_SPECULAR:
							Ret->SetImage(Material::EImageType::Specular, ImagePath.c_str());
							break;
						case aiTextureType::aiTextureType_AMBIENT:
							Ret->SetImage(Material::EImageType::Ambient, ImagePath.c_str());
							break;
						case aiTextureType::aiTextureType_EMISSIVE:
							Ret->SetImage(Material::EImageType::Emissive, ImagePath.c_str());
							break;
						case aiTextureType::aiTextureType_HEIGHT:
							Ret->SetImage(Material::EImageType::Height, ImagePath.c_str());
							break;
						case aiTextureType::aiTextureType_NORMALS:
							Ret->SetImage(Material::EImageType::Normal, ImagePath.c_str());
							break;
						case aiTextureType::aiTextureType_SHININESS:
							Ret->SetImage(Material::EImageType::Shininess, ImagePath.c_str());
							break;
						case aiTextureType::aiTextureType_OPACITY:
							Ret->SetImage(Material::EImageType::Opacity, ImagePath.c_str());
							break;
						case aiTextureType::aiTextureType_DISPLACEMENT:
							Ret->SetImage(Material::EImageType::Displacement, ImagePath.c_str());
							break;
						case aiTextureType::aiTextureType_LIGHTMAP:
							Ret->SetImage(Material::EImageType::Lightmap, ImagePath.c_str());
							break;
						case aiTextureType::aiTextureType_REFLECTION:
							Ret->SetImage(Material::EImageType::Reflection, ImagePath.c_str());
							break;
						case aiTextureType::aiTextureType_METALNESS:
							Ret->SetImage(Material::EImageType::Metalness, ImagePath.c_str());
							break;
						case aiTextureType::aiTextureType_DIFFUSE_ROUGHNESS:
							Ret->SetImage(Material::EImageType::Roughness, ImagePath.c_str());
							break;
						case aiTextureType::aiTextureType_AMBIENT_OCCLUSION:
							Ret->SetImage(Material::EImageType::Occlusion, ImagePath.c_str());
							break;
						case aiTextureType::aiTextureType_SHEEN:
							Ret->SetImage(Material::EImageType::Sheen, ImagePath.c_str());
							break;
						case aiTextureType::aiTextureType_CLEARCOAT:
							Ret->SetImage(Material::EImageType::ClearCoat, ImagePath.c_str());
							break;
						case aiTextureType::aiTextureType_TRANSMISSION:
							Ret->SetImage(Material::EImageType::Transmission, ImagePath.c_str());
							break;
						default:
							Ret->SetImage(Material::EImageType::Unknown, ImagePath.c_str());
							break;
						}
					}
				}
			}

			Ret->Save();
			return Ret;
		}
	}

	const char8_t* DefaultMaterial = "Default.json";
	std::string Path = StringUtils::Format("%s%s", ASSET_PATH_MATERIALS, DefaultMaterial);

	if (!File::Exists(Path.c_str()))
	{
		auto TempMaterial = std::make_shared<Material>(DefaultMaterial);
		TempMaterial->SetShadingMode(Material::EShadingMode::BlinnPhong);
		TempMaterial->Save();
		return TempMaterial;
	}

	return Material::Load(DefaultMaterial);
}

std::pair<std::shared_ptr<Mesh>, std::shared_ptr<Material>> AssimpImporter::ProcessMesh(const aiScene* AssimpScene, const aiMesh* AssimpMesh, ModelAsset* Model)
{
	assert(AssimpMesh && AssimpMesh->HasPositions());

	std::pair<std::shared_ptr<Mesh>, std::shared_ptr<Material>> Ret;

	RHI::EPrimitiveTopology Topology = RHI::EPrimitiveTopology::TriangleList;
	switch (AssimpMesh->mPrimitiveTypes)
	{
	case aiPrimitiveType_POINT:
		assert(0);
		Topology = RHI::EPrimitiveTopology::PointList;
		break;
	case aiPrimitiveType_LINE:
		assert(0);
		Topology = RHI::EPrimitiveTopology::LineList;
		break;
	case aiPrimitiveType_TRIANGLE:
		Topology = RHI::EPrimitiveTopology::TriangleList;
		break;
	case aiPrimitiveType_POLYGON:
		assert(0);
		break;
	}

	AABB BoundingBox = AABB(
		Vector3(AssimpMesh->mAABB.mMin.x, AssimpMesh->mAABB.mMin.y, AssimpMesh->mAABB.mMin.z),
		Vector3(AssimpMesh->mAABB.mMax.x, AssimpMesh->mAABB.mMax.y, AssimpMesh->mAABB.mMax.z));

	std::vector<uint32_t> UVs;
	std::vector<uint32_t> Colors;
	for (uint32_t UVIndex = 0u; UVIndex < AI_MAX_NUMBER_OF_TEXTURECOORDS; ++UVIndex)
	{
		if (UVs.size() >= Mesh::VertexLayout::ELimits::MaxUVs)
		{
			break;
		}
		if (AssimpMesh->HasTextureCoords(UVIndex))
		{
			UVs.push_back(UVIndex);
		}
	}
	for (uint32_t ColorIndex = 0u; ColorIndex < AI_MAX_NUMBER_OF_COLOR_SETS; ++ColorIndex)
	{
		if (Colors.size() >= Mesh::VertexLayout::ELimits::MaxColors)
		{
			break;
		}
		if (AssimpMesh->HasVertexColors(ColorIndex))
		{
			Colors.push_back(ColorIndex);
		}
	}

	Mesh::VertexLayout Layout(
		AssimpMesh->mNumVertices,
		AssimpMesh->mNumFaces * 3u,
		AssimpMesh->HasNormals(),
		AssimpMesh->HasTangentsAndBitangents(),
		static_cast<uint32_t>(UVs.size()),
		static_cast<uint32_t>(Colors.size()));

	assert(AssimpMesh->HasFaces());
	for (uint32_t FaceIndex = 0u; FaceIndex < AssimpMesh->mNumFaces; ++FaceIndex)
	{
		const auto& Face = AssimpMesh->mFaces[FaceIndex];
		assert(Face.mNumIndices == 3u);
		Layout.SetFace(FaceIndex, Face.mIndices[0], Face.mIndices[1], Face.mIndices[2]);
	}

	for (uint32_t VertexIndex = 0u; VertexIndex < AssimpMesh->mNumVertices; ++VertexIndex)
	{
		const auto& Position = AssimpMesh->mVertices[VertexIndex];
		Layout.SetPosition(VertexIndex, Vector3(Position.x, Position.y, Position.z));
		if (AssimpMesh->HasNormals())
		{
			const auto& Normal = AssimpMesh->mNormals[VertexIndex];
			Layout.SetNormal(VertexIndex, Vector3(Normal.x, Normal.y, Normal.z));
		}
		if (AssimpMesh->HasTangentsAndBitangents())
		{
			const auto& Tangent = AssimpMesh->mTangents[VertexIndex];
			Layout.SetTangent(VertexIndex, Vector3(Tangent.x, Tangent.y, Tangent.z));

			const auto& Bitangent = AssimpMesh->mBitangents[VertexIndex];
			Layout.SetBitangent(VertexIndex, Vector3(Bitangent.x, Bitangent.y, Bitangent.z));
		}
		for (uint32_t UVIndex = 0u; UVIndex < UVs.size(); ++UVIndex)
		{
			auto Index = UVs[UVIndex];
			const auto& UV = AssimpMesh->mTextureCoords[Index][VertexIndex];
			Layout.SetUV(VertexIndex, Index, Vector3(UV.x, UV.y, UV.z));
		}
		for (uint32_t ColorIndex = 0u; ColorIndex < Colors.size(); ++ColorIndex)
		{
			auto Index = Colors[ColorIndex];
			const auto& ColorValue = AssimpMesh->mColors[Index][VertexIndex];
			Layout.SetColor(VertexIndex, ColorIndex, Color(ColorValue.r, ColorValue.g, ColorValue.b, ColorValue.a));
		}
	}

	Ret.second = ProcessMaterial(AssimpScene, AssimpMesh, Model);

	Ret.first = std::make_shared<Mesh>(
		Layout,
		Topology,
		*Ret.second->Shader(RHI::EShaderStage::Vertex)->Desc(),
		BoundingBox,
		AssimpMesh->mName.C_Str());

	return Ret;
}
#endif
