#pragma once

#include "Engine/Asset/SceneAsset.h"
#include "Engine/Asset/AssetDatabase.h"
#include "Engine/Paths.h"
#include <assimp/version.h>
#include <Submodules/assimp/include/assimp/Importer.hpp>
#include <Submodules/assimp/include/assimp/ProgressHandler.hpp>
#include <Submodules/assimp/include/assimp/scene.h>
#include <Submodules/assimp/include/assimp/GltfMaterial.h>
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
		LOG_INFO("Create Assimp scene importer, assimp version: {}.{}.{}", aiGetVersionMajor(), aiGetVersionMinor(), aiGetVersionPatch());
	}

	void LoadAssetData(std::shared_ptr<Asset>&, AssetType::EContentsType) override final {} /// Just load from file to avoid base path of texture broken

	std::shared_ptr<Asset> CreateAsset(const std::filesystem::path& AssetPath) override final { return std::make_shared<AssimpSceneAsset>(AssetPath); }

	bool Reimport(Asset& InAsset, const AssetType& InAssetType) override final
	{
		auto& AssimpScene = Cast<AssimpSceneAsset>(InAsset);

		const uint32_t ProcessFlags = static_cast<uint32_t>(
			aiProcessPreset_TargetRealtime_MaxQuality | 
			aiProcess_ConvertToLeftHanded |  /// Use DirectX's left-hand coordinate system
			aiProcess_GenBoundingBoxes |
			aiProcess_TransformUVCoords);

		Assimp::Importer AssimpImporter;

#if _DEBUG
		Assimp::DefaultLogger::set(new AssimpLogger());
		AssimpImporter.SetProgressHandler(new AssimpProgressHandler(AssimpScene.GetPath()));
#endif
		if (auto AiScene = AssimpImporter.ReadFile(AssimpScene.GetPath().string(), ProcessFlags))
		{
			if (AiScene->HasMeshes())
			{
				ProcessMaterials(AiScene, AssimpScene);

				ProcessMeshes(AiScene, AssimpScene);

				if (AiScene->mRootNode)
				{
					AssimpScene.Graph.Root = AssimpScene.Graph.AddNode(SceneGraph::NodeID(), AiScene->mRootNode->mName.C_Str());
					if (ProcessNode(AiScene, AiScene->mRootNode, AssimpScene.Graph.Root, AssimpScene))
					{
#if !ENABLE_MULTI_RENDERER
						assert(false);
#endif
						return true;
					}
				}
				return false;
			}
			else
			{
				LOG_CAT_WARNING(LogAssimpImporter, "Assimp scene: \"{}\" has no meshes");
				return true;
			}
		}

		LOG_CAT_ERROR(LogAssimpImporter, "Failed to load assimp scene: {}, error message: {}", AssimpScene.GetPath().generic_string(), AssimpImporter.GetErrorString());
		return false;
	}

private:
	class AssimpProgressHandler : public Assimp::ProgressHandler
	{
	public:
		AssimpProgressHandler(const std::filesystem::path& AssetPath)
			: m_AssetPath(AssetPath)
		{
		}

		bool Update(float Percentage) override final
		{
			if (Percentage >= 1.0f)
			{
				LOG_CAT_DEBUG(LogAssimpImporter, "Loading model \"{}\" succeeded", m_AssetPath.generic_string());
				return true;
			}

			if (static_cast<int32_t>(Percentage * 100) % 10 == 0)
			{
				LOG_CAT_DEBUG(LogAssimpImporter, "Loading model: \"{}\" in progress {:.2f}%", m_AssetPath.generic_string(), Percentage * 100);
			}
			return false;
		}
	private:
		const std::filesystem::path& m_AssetPath;
	};

	class AssimpLogger : public Assimp::Logger
	{
	public:
		void OnDebug(const char* Message) override final { LOG_CAT_DEBUG(LogAssimpImporter, Message); }
		void OnVerboseDebug(const char* Message) override final { LOG_CAT_DEBUG(LogAssimpImporter, Message); }
		void OnInfo(const char* Message) override final { LOG_CAT_INFO(LogAssimpImporter, Message); }
		void OnWarn(const char* Message) override final { LOG_CAT_WARNING(LogAssimpImporter, Message); }
		void OnError(const char* Message) override final { LOG_CAT_ERROR(LogAssimpImporter, Message); }
		bool attachStream(Assimp::LogStream*, uint32_t) override final { return true; }
		bool detachStream(Assimp::LogStream*, uint32_t) override final { return true; }
	};

	bool ProcessNode(const aiScene* AiScene, const aiNode* AiNode, SceneGraph::NodeID GraphNode, AssimpSceneAsset& AssimpScene)
	{
		if (!AiNode)
		{
			return false;
		}

		auto Node = AiScene->mRootNode == AiNode ? GraphNode : AssimpScene.Graph.AddChild(GraphNode, AiNode->mName.C_Str());
		for (uint32_t Index = 0u; Index < AiNode->mNumMeshes; ++Index)
		{
			const auto MeshIndex = AiNode->mMeshes[Index];
			const auto Mesh = AiScene->mMeshes[MeshIndex];

			auto GraphNodeID = AssimpScene.Graph.AddChild(GraphNode, Mesh->mName.C_Str(), SceneGraph::Node::ENodeMasks::Primitive);

			if (Mesh->HasBones())
			{
				assert(false);
			}
			else
			{
				if (auto SceneNode = const_cast<SceneGraph::Node*>(AssimpScene.Graph.GetNode(GraphNodeID)))
				{
					SceneNode->SetDataIndex(MeshIndex);
				}
			}
		}

		for (uint32_t NodeIndex = 0u; NodeIndex < AiNode->mNumChildren; ++NodeIndex)
		{
			if (!ProcessNode(AiScene, AiNode->mChildren[NodeIndex], Node, AssimpScene))
			{
				return false;
			}
		}

		return true;
	}

	void ProcessMaterials(const aiScene* AiScene, AssimpSceneAsset& AssimpScene)
	{
		if (!AiScene->HasMaterials())
		{
			return;
		}

		AssimpScene.Data.MaterialProperties.resize(AiScene->mNumMaterials);
		for (uint32_t Index = 0u; Index < AiScene->mNumMaterials; ++Index)
		{
			if (auto AiMaterial = AiScene->mMaterials[Index])
			{
				aiString Name;
				AiMaterial->Get(AI_MATKEY_NAME, Name);

				auto MaterialPath = (Paths::MaterialPath() / AssimpScene.GetStem() / Name.C_Str()).replace_extension(MaterialProperty::GetExtension());
				auto NeedReload = std::filesystem::exists(MaterialPath);
				auto& Property = AssimpScene.Data.MaterialProperties.at(Index);
				Property = MaterialProperty::Load(MaterialPath);

				if (NeedReload)
				{
					aiString AlphaMode;
					AiMaterial->Get(AI_MATKEY_GLTF_ALPHAMODE, AlphaMode);

					aiShadingMode ShadingMode = aiShadingMode_Unlit;
					AiMaterial->Get(AI_MATKEY_SHADING_MODEL, ShadingMode);

					Property->Name.assign(Name.C_Str());

#define GET_COLOR_FACTOR(Key, Target) { aiColor4D Factor(1.0f, 1.0f, 1.0f, 1.0f); AiMaterial->Get(Key, Factor); Target = Math::Color(Factor.r, Factor.g, Factor.b, Factor.a); }
#define GET_FACTOR(Key, Target) { ai_real Factor = 1.0f; AiMaterial->Get(Key, Factor); Target = Factor; }

					GET_COLOR_FACTOR(AI_MATKEY_BASE_COLOR, Property->Factors.BaseColor);
					GET_COLOR_FACTOR(AI_MATKEY_COLOR_DIFFUSE, Property->Factors.DiffuseColor);
					GET_COLOR_FACTOR(AI_MATKEY_COLOR_SPECULAR, Property->Factors.SpecularColor);
					GET_COLOR_FACTOR(AI_MATKEY_COLOR_EMISSIVE, Property->Factors.EmissiveColor);
					GET_COLOR_FACTOR(AI_MATKEY_COLOR_TRANSPARENT, Property->Factors.TransparentColor);
					GET_COLOR_FACTOR(AI_MATKEY_COLOR_REFLECTIVE, Property->Factors.ReflectiveColor);

					GET_FACTOR(AI_MATKEY_METALLIC_FACTOR, Property->Factors.Metalness);
					GET_FACTOR(AI_MATKEY_ROUGHNESS_FACTOR, Property->Factors.Roughness);
					GET_FACTOR(AI_MATKEY_GLOSSINESS_FACTOR, Property->Factors.Glossiness);
					GET_FACTOR(AI_MATKEY_SPECULAR_FACTOR, Property->Factors.Specular);
					GET_FACTOR(AI_MATKEY_OPACITY, Property->Factors.Opacity);
					GET_FACTOR(AI_MATKEY_SHININESS, Property->Factors.Shininess);

					AiMaterial->Get(AI_MATKEY_GLTF_ALPHACUTOFF, Property->AlphaCutoff);
					AiMaterial->Get(AI_MATKEY_TWOSIDED, Property->DoubleSided);
#undef GET_COLOR_FACTOR
#undef GET_FACTOR

					switch (ShadingMode)
					{
					case aiShadingMode_Flat:
					case aiShadingMode_Gouraud:
					case aiShadingMode_Phong:
					case aiShadingMode_Blinn:
						Property->ShadingMode = EShadingMode::BlinnPhong;
						break;
					case aiShadingMode_Toon:
						Property->ShadingMode = EShadingMode::Toon;
						break;
					case aiShadingMode_OrenNayar:
					case aiShadingMode_Minnaert:
					case aiShadingMode_CookTorrance:
					case aiShadingMode_Fresnel:
					case aiShadingMode_PBR_BRDF:
						Property->ShadingMode = EShadingMode::StandardPBR;
						break;
					case aiShadingMode_NoShading:
						Property->ShadingMode = EShadingMode::Unlit;
						break;
					}
				}

				ProcessTextures(AiMaterial, *Property, AssimpScene.GetPath().parent_path());
			}
		}
	}

	void ProcessMeshes(const aiScene* AiScene, AssimpSceneAsset& AssimpScene)
	{
		for (uint32_t Index = 0u; Index < AiScene->mNumMeshes; ++Index)
		{
			const auto Mesh = AiScene->mMeshes[Index];

			if (!Mesh)
			{
				LOG_CAT_ERROR(LogAssimpImporter, "Detected invalid mesh!");
				continue;
			}
			if (!Mesh->HasPositions())
			{
				LOG_CAT_ERROR(LogAssimpImporter, "The mesh has no vertices data!");
				continue;
			}
			if (!Mesh->HasNormals())
			{
				LOG_CAT_WARNING(LogAssimpImporter, "The mesh has no normals!");
				continue;
			}
			if (!Mesh->HasFaces())
			{
				LOG_CAT_ERROR(LogAssimpImporter, "The mesh has no indices data!");
				continue;
			}
			if (Mesh->mPrimitiveTypes != aiPrimitiveType_TRIANGLE)
			{
				LOG_CAT_ERROR(LogAssimpImporter, "Detected others primitive type, should never be happen!");
				continue;
			}

			Math::AABB BoundingBox = Math::AABB(
				Math::Vector3(Mesh->mAABB.mMin.x, Mesh->mAABB.mMin.y, Mesh->mAABB.mMin.z),
				Math::Vector3(Mesh->mAABB.mMax.x, Mesh->mAABB.mMax.y, Mesh->mAABB.mMax.z));

			MeshData MeshDataBlock(
				Mesh->mNumVertices,
				Mesh->mNumFaces * 3u,
				Mesh->mNumFaces,
				Mesh->HasNormals(),
				Mesh->HasTangentsAndBitangents(),
				Mesh->HasTextureCoords(0u),
				Mesh->HasTextureCoords(1u),
				Mesh->HasVertexColors(0u),
				ERHIPrimitiveTopology::TriangleList,
				BoundingBox);

			for (uint32_t FaceIndex = 0u; FaceIndex < Mesh->mNumFaces; ++FaceIndex)
			{
				const auto& Face = Mesh->mFaces[FaceIndex];
				assert(Face.mNumIndices == 3u);
				MeshDataBlock.SetFace(FaceIndex, Face.mIndices[0], Face.mIndices[1], Face.mIndices[2]);
			}

			for (uint32_t VertexIndex = 0u; VertexIndex < Mesh->mNumVertices; ++VertexIndex)
			{
				const auto& Position = Mesh->mVertices[VertexIndex];
				MeshDataBlock.SetPosition(VertexIndex, Math::Vector3(Position.x, Position.y, Position.z));

				if (Mesh->HasNormals())
				{
					const auto& Normal = Mesh->mNormals[VertexIndex];
					MeshDataBlock.SetNormal(VertexIndex, Math::Vector3(Normal.x, Normal.y, Normal.z));
				}
				if (Mesh->HasTangentsAndBitangents())
				{
					const auto& Tangent = Mesh->mTangents[VertexIndex];
					MeshDataBlock.SetTangent(VertexIndex, Math::Vector3(Tangent.x, Tangent.y, Tangent.z));

					const auto& Bitangent = Mesh->mBitangents[VertexIndex];
					MeshDataBlock.SetBitangent(VertexIndex, Math::Vector3(Bitangent.x, Bitangent.y, Bitangent.z));
				}

				if (Mesh->HasTextureCoords(0u))
				{
					const auto& UV = Mesh->mTextureCoords[0u][VertexIndex];
					MeshDataBlock.SetUV0(VertexIndex, Math::Vector3(UV.x, UV.y, UV.z));
				}
				if (Mesh->HasTextureCoords(1))
				{
					const auto& UV = Mesh->mTextureCoords[1u][VertexIndex];
					MeshDataBlock.SetUV0(VertexIndex, Math::Vector3(UV.x, UV.y, UV.z));
				}

				if (Mesh->HasVertexColors(0u))
				{
					const auto& Color = Mesh->mColors[0u][VertexIndex];
					MeshDataBlock.SetColor(VertexIndex, Math::Color(Color.r, Color.g, Color.b, Color.a));
				}
			}

			if (Mesh->HasBones())
			{
				assert(false);
			}
			else
			{
				AssimpScene.Data.StaticMeshes.emplace_back(std::make_shared<StaticMesh>(MeshDataBlock, Mesh->mMaterialIndex))
					->SetMaterialProperty(AssimpScene.Data.MaterialProperties.at(Index).get());
			}
		}
	}

	MaterialProperty::ETextureType GetTextureType(aiTextureType Type)
	{
		switch (Type)
		{
		case aiTextureType_DIFFUSE:
			return MaterialProperty::ETextureType::Diffuse;
		case aiTextureType_SPECULAR:
			return MaterialProperty::ETextureType::Specular;
		case aiTextureType_AMBIENT:
			return MaterialProperty::ETextureType::Ambient;
		case aiTextureType_EMISSIVE:
			return MaterialProperty::ETextureType::Emissive;
		case aiTextureType_HEIGHT:
			return MaterialProperty::ETextureType::Height;
		case aiTextureType_NORMALS:
			return MaterialProperty::ETextureType::Normal;
		case aiTextureType_SHININESS:
			return MaterialProperty::ETextureType::Shininess;
		case aiTextureType_OPACITY:
			return MaterialProperty::ETextureType::Opacity;
		case aiTextureType_DISPLACEMENT:
			return MaterialProperty::ETextureType::Displacement;
		case aiTextureType_LIGHTMAP:
			return MaterialProperty::ETextureType::Lightmap;
		case aiTextureType_REFLECTION:
			return MaterialProperty::ETextureType::Reflection;
		case aiTextureType_BASE_COLOR:
			return MaterialProperty::ETextureType::BaseColor;
		case aiTextureType_EMISSION_COLOR:
			return MaterialProperty::ETextureType::EmissionColor;
		case aiTextureType_METALNESS:
			return MaterialProperty::ETextureType::Metalness;
		case aiTextureType_DIFFUSE_ROUGHNESS:
			return MaterialProperty::ETextureType::DiffuseRoughness;
		case aiTextureType_AMBIENT_OCCLUSION:
			return MaterialProperty::ETextureType::AmbientOcclusion;
		default:
			return MaterialProperty::ETextureType::Num;
		}
	}

	void ProcessTextures(const aiMaterial* AiMaterial, MaterialProperty& Material, const std::filesystem::path& RootPath)
	{
		for (uint32_t Index = aiTextureType_DIFFUSE; Index < aiTextureType_TRANSMISSION; ++Index)
		{
			auto TextureType = static_cast<aiTextureType>(Index);
			if (AiMaterial->GetTextureCount(TextureType) == 0u || TextureType == aiTextureType_UNKNOWN)
			{
				continue;
			}

			aiString URL;
			if (AI_SUCCESS == AiMaterial->GetTexture(TextureType, 0u, &URL))  /// TODO: How to do if the material has more than one textures for some texture type ???
			{
				std::filesystem::path TexturePath = RootPath / URL.C_Str();
				if (std::filesystem::exists(TexturePath))
				{
					auto AssetLoadCallbacks = std::make_optional(Asset::Callbacks{});
					AssetLoadCallbacks.value().PreLoadCallback = [this, &TextureType](Asset& InAsset) {
						Cast<TextureAsset>(InAsset).SetLinear(TextureType != aiTextureType_DIFFUSE && TextureType != aiTextureType_BASE_COLOR);
					};

					auto TextureIndex = GetTextureType(TextureType);
					if (TextureIndex != MaterialProperty::ETextureType::Num)
					{
						auto& Texture = Material.Textures[TextureIndex];
						Texture.reset(new TextureAsset(TexturePath));

						auto TextureAsset = Cast<Asset>(Texture);
						AssetDatabase::Get().GetOrReimportAsset(TextureAsset, AssetLoadCallbacks);
					}
				}
			}
		}
	}
};
