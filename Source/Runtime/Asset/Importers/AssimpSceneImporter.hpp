#pragma once

#include "Asset/SceneAsset.h"
#include "Services/AssetDatabase.h"
#include "Paths.h"
#include "RHI/RHIBackend.h"
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
		: IAssetImporter(
			{
				AssetType{ "Autodesk 3dsMax", ".3ds" },
				AssetType{ "Blender 3D", ".blend" },
				AssetType{ "OpenCOLLADA", ".dae", AssetType::EContentsType::Text },
				AssetType{ "Autodesk FBX", ".fbx"},
				AssetType{ "Graphics Language Transmission Format", ".gltf", AssetType::EContentsType::Text },
				AssetType{ "Wavefront", ".obj", AssetType::EContentsType::Text },
				AssetType{ "Polygon File Format", ".ply", AssetType::EContentsType::Text },
				AssetType{ "Stereolithography", ".stl", AssetType::EContentsType::Text },
				AssetType{ "XFile", ".x", AssetType::EContentsType::Text} 
			})
	{
		LOG_INFO("Create Assimp scene importer, assimp version: {}.{}.{}", aiGetVersionMajor(), aiGetVersionMinor(), aiGetVersionPatch());
	}

	std::shared_ptr<Asset> CreateAsset(const std::filesystem::path& AssetPath) override final { return std::make_shared<AssimpSceneAsset>(AssetPath); }

	bool Reimport(Asset& InAsset, const AssetType&) override final
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
			if (AiScene->HasMeshes() && AiScene->mRootNode)
			{
				AssimpScene.Graph.SetRoot(AssimpScene.Graph.AddEntity(EntityID(), AiScene->mRootNode->mName.C_Str()));
				if (ProcessNode(AiScene, AiScene->mRootNode, AssimpScene.Graph.GetRoot(), AssimpScene))
				{
					ProcessMaterials(AiScene, AssimpScene);
					ProcessMeshes(AiScene, AssimpScene);

					return true;
				}

				return false;
			}
			else
			{
				LOG_CAT_WARNING(LogAssimpImporter, "Assimp scene: \"{}\" has no meshes");
				return true;
			}
		}

		LOG_CAT_ERROR(LogAssimpImporter, "Failed to load assimp scene: {}: \"{}\"", AssimpScene.GetName().generic_string(), AssimpImporter.GetErrorString());
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
				LOG_CAT_DEBUG(LogAssimpImporter, "Loading assimp scene \"{}\" completed", m_AssetPath.generic_string());
				return true;
			}

			if (static_cast<int32_t>(Percentage * 100) % 10 == 0)
			{
				LOG_CAT_DEBUG(LogAssimpImporter, "Loading assimp scene: \"{}\" in progress {:.2f}%", m_AssetPath.generic_string(), Percentage * 100);
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

	void ProcessTransform(const aiNode* AiNode, AssimpSceneAsset& AssimpScene)
	{
		std::stack<const aiMatrix4x4*> RelativeTransforms;
		auto Node = AiNode;
		while (Node)
		{
			RelativeTransforms.push(&Node->mTransformation);
			Node = Node->mParent;
		}

		aiMatrix4x4 FinalTransform = *RelativeTransforms.top();
		RelativeTransforms.pop();
		while (!RelativeTransforms.empty())
		{
			FinalTransform *= *RelativeTransforms.top();
			RelativeTransforms.pop();
		}

		aiVector3D Translation;
		aiVector3D Scalling;
		aiQuaternion Rotation;
		FinalTransform.Decompose(Scalling, Rotation, Translation);
		AssimpScene.Transforms.emplace_back(
			Math::Transform(
				Math::Vector3(Translation.x, Translation.y, Translation.z),
				Math::Vector3(Scalling.x, Scalling.y, Scalling.z),
				Math::Quaternion(Rotation.x, Rotation.y, Rotation.z, Rotation.w)));
	}

	bool ProcessNode(const aiScene* AiScene, const aiNode* AiNode, EntityID GraphNode, AssimpSceneAsset& AssimpScene)
	{
		if (!AiNode)
		{
			return false;
		}

		auto Node = AiScene->mRootNode == AiNode ? GraphNode : AssimpScene.Graph.AddChild(GraphNode, AiNode->mName.C_Str());
		for (uint32_t Index = 0u; Index < AiNode->mNumMeshes; ++Index)
		{
			const auto MeshIndex = AiNode->mMeshes[Index];
			const auto AiMesh = AiScene->mMeshes[MeshIndex];

			if (AiMesh->HasBones())
			{
				LOG_CAT_ERROR(LogAssimpImporter, "Not supported yet!");
				assert(false);
			}

			auto NodeID = AssimpScene.Graph.AddChild(GraphNode, AiMesh->mName.C_Str());
			auto& DataIndices = AssimpScene.EntityDataIndices.insert(std::make_pair(NodeID, AssimpSceneAsset::DataIndex{})).first->second;
			DataIndices.Mesh = MeshIndex;
			DataIndices.Material = AiMesh->mMaterialIndex;
			DataIndices.Transfrom = static_cast<uint32_t>(AssimpScene.Transforms.size());

			ProcessTransform(AiNode, AssimpScene);
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

		AssimpScene.MaterialProperties.resize(AiScene->mNumMaterials);
		for (uint32_t Index = 0u; Index < AiScene->mNumMaterials; ++Index)
		{
			if (auto AiMaterial = AiScene->mMaterials[Index])
			{
				aiString Name;
				AiMaterial->Get(AI_MATKEY_NAME, Name);

				auto MaterialPath = (Paths::MaterialPath() / AssimpScene.GetStem() / Name.C_Str()).replace_extension(MaterialProperty::GetExtension());
				auto NeedReload = std::filesystem::exists(MaterialPath);
				auto& Property = AssimpScene.MaterialProperties.at(Index);
				Property = MaterialProperty::Load(MaterialPath);

				if (NeedReload)
				{
					aiString AlphaMode;
					if (AiMaterial->Get(AI_MATKEY_GLTF_ALPHAMODE, AlphaMode) == AI_SUCCESS)
					{
						if (AlphaMode == aiString("OPAQUE"))
						{
							Property->BlendMode = EBlendMode::Opaque;
						}
						else if (AlphaMode == aiString("MASK"))
						{
							Property->BlendMode = EBlendMode::Masked;
						}
						else if (AlphaMode == aiString("BLEND"))
						{
							Property->BlendMode = EBlendMode::Translucent;
						}
					}

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
			const auto AiMesh = AiScene->mMeshes[Index];

			if (!AiMesh)
			{
				LOG_CAT_ERROR(LogAssimpImporter, "Detected invalid mesh!");
				continue;
			}
			if (!AiMesh->HasPositions())
			{
				LOG_CAT_ERROR(LogAssimpImporter, "The mesh has no vertices data!");
				continue;
			}
			if (!AiMesh->HasNormals())
			{
				LOG_CAT_WARNING(LogAssimpImporter, "The mesh has no normals!");
				continue;
			}
			if (!AiMesh->HasFaces())
			{
				LOG_CAT_ERROR(LogAssimpImporter, "The mesh has no indices data!");
				continue;
			}
			if (AiMesh->mPrimitiveTypes != aiPrimitiveType_TRIANGLE)
			{
				LOG_CAT_ERROR(LogAssimpImporter, "Detected others primitive type, should never be happen!");
				continue;
			}

			Math::AABB BoundingBox = Math::AABB(
				Math::Vector3(AiMesh->mAABB.mMin.x, AiMesh->mAABB.mMin.y, AiMesh->mAABB.mMin.z),
				Math::Vector3(AiMesh->mAABB.mMax.x, AiMesh->mAABB.mMax.y, AiMesh->mAABB.mMax.z));

			MeshData MeshDataBlock(
				AiMesh->mNumVertices,
				AiMesh->mNumFaces * 3u,
				AiMesh->mNumFaces,
				AiMesh->HasNormals(),
				AiMesh->HasTangentsAndBitangents(),
				AiMesh->HasTextureCoords(0u),
				AiMesh->HasTextureCoords(1u),
				AiMesh->HasVertexColors(0u),
				ERHIPrimitiveTopology::TriangleList,
				BoundingBox,
				AiMesh->mName.C_Str());

			for (uint32_t FaceIndex = 0u; FaceIndex < AiMesh->mNumFaces; ++FaceIndex)
			{
				const auto& Face = AiMesh->mFaces[FaceIndex];
				assert(Face.mNumIndices == 3u);
				MeshDataBlock.SetFace(FaceIndex, Face.mIndices[0], Face.mIndices[1], Face.mIndices[2]);
			}

			for (uint32_t VertexIndex = 0u; VertexIndex < AiMesh->mNumVertices; ++VertexIndex)
			{
				const auto& Position = AiMesh->mVertices[VertexIndex];
				MeshDataBlock.SetPosition(VertexIndex, Math::Vector3(Position.x, Position.y, Position.z));

				if (AiMesh->HasNormals())
				{
					const auto& Normal = AiMesh->mNormals[VertexIndex];
					MeshDataBlock.SetNormal(VertexIndex, Math::Vector3(Normal.x, Normal.y, Normal.z));
				}
				if (AiMesh->HasTangentsAndBitangents())
				{
					const auto& Tangent = AiMesh->mTangents[VertexIndex];
					MeshDataBlock.SetTangent(VertexIndex, Math::Vector3(Tangent.x, Tangent.y, Tangent.z));

					const auto& Bitangent = AiMesh->mBitangents[VertexIndex];
					MeshDataBlock.SetBitangent(VertexIndex, Math::Vector3(Bitangent.x, Bitangent.y, Bitangent.z));
				}

				if (AiMesh->HasTextureCoords(0u))
				{
					const auto& UV = AiMesh->mTextureCoords[0u][VertexIndex];
					MeshDataBlock.SetUV0(VertexIndex, Math::Vector3(UV.x, UV.y, UV.z));
				}
				if (AiMesh->HasTextureCoords(1))
				{
					const auto& UV = AiMesh->mTextureCoords[1u][VertexIndex];
					MeshDataBlock.SetUV0(VertexIndex, Math::Vector3(UV.x, UV.y, UV.z));
				}

				if (AiMesh->HasVertexColors(0u))
				{
					const auto& Color = AiMesh->mColors[0u][VertexIndex];
					MeshDataBlock.SetColor(VertexIndex, Math::Color(Color.r, Color.g, Color.b, Color.a));
				}
			}

			if (AiMesh->HasBones())
			{
				assert(false);
			}
			else
			{
				auto& Mesh = AssimpScene.StaticMeshes.emplace_back(std::make_shared<StaticMesh>(MeshDataBlock));
				//Mesh->CreateRHI(MeshDataBlock, );
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
		std::unordered_map<std::filesystem::path, std::shared_ptr<TextureAsset>> Textures;

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
					auto TextureIndex = GetTextureType(TextureType);
					if (TextureIndex != MaterialProperty::ETextureType::Num)
					{
						auto It = Textures.find(TexturePath);
						if (It == Textures.end())
						{
							auto& Texture = Material.Textures[TextureIndex];
							Texture.reset(new TextureAsset(TexturePath));

							It = Textures.emplace(TexturePath, Texture).first;

							auto AssetLoadCallbacks = std::make_optional<Asset::AssetLoadCallbacks>();
							AssetLoadCallbacks.value().PreLoadCallback = [this, &TextureType](Asset& InAsset) {
								Cast<TextureAsset>(InAsset).SetLinear(TextureType != aiTextureType_DIFFUSE && TextureType != aiTextureType_BASE_COLOR);
								};

							AssetDatabase::Get().GetOrReimportAsset<TextureAsset>(Texture->GetPath(), AssetLoadCallbacks);
						}
						else
						{
							Material.Textures[TextureIndex] = It->second;
						}
					}
				}
			}
		}
	}
};
