#pragma once

#include "Runtime/Engine/Asset/SceneAsset.h"
#include "Runtime/Engine/Asset/AssetDatabase.h"
#include "Runtime/Engine/Asset/ImageAsset.h"
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

	void LoadAssetData(std::shared_ptr<Asset>&, AssetType::EContentsType) override final {} /// Just load from file to avoid base path of texture broken

	std::shared_ptr<Asset> CreateAsset(const std::filesystem::path& AssetPath) override final { return std::make_shared<AssimpScene>(AssetPath); }

	bool8_t Reimport(Asset& InAsset) override final
	{
		auto& Scene = Cast<AssimpScene>(InAsset);

		const uint32_t ProcessFlags = static_cast<uint32_t>(
			aiProcessPreset_TargetRealtime_MaxQuality | 
			aiProcess_MakeLeftHanded |  /// Use DirectX's left-hand coordinate system
			aiProcess_FlipUVs |
			aiProcess_FlipWindingOrder |
			aiProcess_GenBoundingBoxes |
			aiProcess_TransformUVCoords);

		Assimp::Importer AssimpImporter;

#if _DEBUG
		Assimp::DefaultLogger::set(new AssimpLogger());
		AssimpImporter.SetProgressHandler(new AssimpProgressHandler(Scene.GetPath()));
#endif
		if (auto AiScene = AssimpImporter.ReadFile(Scene.GetPath().u8string(), ProcessFlags))
		{
			if (AiScene->HasMeshes())
			{
				ProcessMaterials(AiScene, Scene);

				if (AiScene->mRootNode)
				{
					Scene.Graph.Root = Scene.Graph.AddNode(SceneGraph::NodeID(), AiScene->mRootNode->mName.C_Str());
					return ProcessNode(AiScene, AiScene->mRootNode, Scene.Graph.Root, Scene);
				}
				return false;
			}
			else
			{
				LOG_WARNING("AssimpSceneImporter:: Assimp scene \"{}\" has no meshes");
				return true;
			}
		}

		LOG_ERROR("AssimpSceneImporter:: Failed to load assimp scene \"{}\" : {}", Scene.GetPath().generic_string(), AssimpImporter.GetErrorString());
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

		bool8_t Update(float32_t Percentage) override final
		{
			if (Percentage >= 1.0f)
			{
				LOG_DEBUG("AssimpSceneImporter: Loading model \"{}\" succeeded", m_AssetPath.generic_string());
				return true;
			}

			if (static_cast<int32_t>(Percentage * 100) % 10 == 0)
			{
				LOG_DEBUG("AssimpSceneImporter: Loading model: \"{}\" in progress {:.2f}%", m_AssetPath.generic_string(), Percentage * 100);
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

	bool8_t ProcessNode(const aiScene* AiScene, const aiNode* AiNode, SceneGraph::NodeID GraphNode, AssimpScene& AssimpScene)
	{
		if (!AiNode)
		{
			return false;
		}

		auto Node = AiScene->mRootNode == AiNode ? GraphNode : AssimpScene.Graph.AddChild(GraphNode, AiNode->mName.C_Str());
		ProcessMeshes(AiScene, AiNode, AssimpScene, Node);

		for (uint32_t NodeIndex = 0u; NodeIndex < AiNode->mNumChildren; ++NodeIndex)
		{
			if (!ProcessNode(AiScene, AiNode->mChildren[NodeIndex], Node, AssimpScene))
			{
				return false;
			}
		}

		return true;
	}

	void ProcessMaterials(const aiScene* AiScene, AssimpScene& AssimpScene)
	{
		if (!AiScene->HasMaterials())
		{
			return;
		}

		AssimpScene.Data.Materials.resize(AiScene->mNumMaterials);
		for (uint32_t Index = 0u; Index < AiScene->mNumMaterials; ++Index)
		{
			if (auto Material = AiScene->mMaterials[Index])
			{
				aiString Name;
				Material->Get(AI_MATKEY_NAME, Name);

				aiShadingMode ShadingMode = aiShadingMode::aiShadingMode_Unlit;
				Material->Get(AI_MATKEY_SHADING_MODEL, ShadingMode);

				aiColor4D BaseColor(1.0f, 1.0f, 1.0f, 1.0f);
				Material->Get(AI_MATKEY_BASE_COLOR, BaseColor);

				aiColor4D DiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
				Material->Get(AI_MATKEY_COLOR_DIFFUSE, DiffuseColor);

				aiColor4D SpecularColor(1.0f, 1.0f, 1.0f, 1.0f);
				Material->Get(AI_MATKEY_COLOR_SPECULAR, SpecularColor);

				aiColor4D EmissiveColor(1.0f, 1.0f, 1.0f, 1.0f);
				Material->Get(AI_MATKEY_COLOR_EMISSIVE, EmissiveColor);

				aiColor4D TransparentColor(1.0f, 1.0f, 1.0f, 1.0f);
				Material->Get(AI_MATKEY_COLOR_TRANSPARENT, TransparentColor);

				aiColor4D ReflectiveColor(1.0f, 1.0f, 1.0f, 1.0f);
				Material->Get(AI_MATKEY_COLOR_REFLECTIVE, ReflectiveColor);

				ai_real MetallicFactor = 1.0f;
				Material->Get(AI_MATKEY_METALLIC_FACTOR, MetallicFactor);

				ai_real RoughnessFactor = 1.0f;
				Material->Get(AI_MATKEY_ROUGHNESS_FACTOR, RoughnessFactor);

				ai_real GlossinessFactor = 1.0f;
				Material->Get(AI_MATKEY_GLOSSINESS_FACTOR, GlossinessFactor);

				ai_real SpecularFactor = 1.0f;
				Material->Get(AI_MATKEY_SPECULAR_FACTOR, SpecularFactor);

				ai_real Opacity = 1.0f;
				Material->Get(AI_MATKEY_OPACITY, Opacity);

				ai_real Shininess = 1.0f;
				Material->Get(AI_MATKEY_SHININESS, Shininess);

				bool8_t TwoSided = false;
				Material->Get(AI_MATKEY_TWOSIDED, TwoSided);

				ProcessTextures(Material, AssimpScene.GetPath().parent_path());
			}
			else
			{
			}
		}
	}

	void ProcessTextures(const aiMaterial* Material, const std::filesystem::path& RootPath)
	{
		for (uint32_t Index = aiTextureType_DIFFUSE; Index < aiTextureType_TRANSMISSION; ++Index)
		{
			auto TextureType = static_cast<aiTextureType>(Index);
			if (Material->GetTextureCount(TextureType) == 0u || TextureType == aiTextureType_UNKNOWN)
			{
				continue;
			}

			aiString Path;
			if (AI_SUCCESS == Material->GetTexture(TextureType, 0u, &Path))  /// TODO: How to do if the material has more than one textures for some texture type ???
			{
				std::filesystem::path TexturePath = RootPath / Path.C_Str();
				if (std::filesystem::exists(TexturePath))
				{
					auto AssetLoadCallbacks = std::make_optional(Asset::Callbacks{});
					AssetLoadCallbacks.value().PreLoadCallback = [this](Asset& InAsset) {
						Cast<ImageAsset>(InAsset);
						/// Set sRGB
					};

					AssetDatabase::Get().FindOrImportAsset<ImageAsset>(TexturePath, AssetLoadCallbacks);

					switch (TextureType)
					{
					case aiTextureType_DIFFUSE:
						break;
					case aiTextureType_SPECULAR:
						break;
					case aiTextureType_AMBIENT:
						break;
					case aiTextureType_EMISSIVE:
						break;
					case aiTextureType_HEIGHT:
						break;
					case aiTextureType_NORMALS:
						break;
					case aiTextureType_SHININESS:
						break;
					case aiTextureType_OPACITY:
						break;
					case aiTextureType_DISPLACEMENT:
						break;
					case aiTextureType_LIGHTMAP:
						break;
					case aiTextureType_REFLECTION:
						break;
					case aiTextureType_BASE_COLOR:
						break;
					case aiTextureType_NORMAL_CAMERA:
						break;
					case aiTextureType_EMISSION_COLOR:
						break;
					case aiTextureType_METALNESS:
						break;
					case aiTextureType_DIFFUSE_ROUGHNESS:
						break;
					case aiTextureType_AMBIENT_OCCLUSION:
						break;
					case aiTextureType_SHEEN:
						break;
					case aiTextureType_CLEARCOAT:
						break;
					case aiTextureType_TRANSMISSION:
						break;
					}
				}
			}
		}
	}

	void ProcessMeshes(const aiScene* AiScene, const aiNode* AiNode, AssimpScene& AssimpScene, SceneGraph::NodeID GraphNode)
	{
		for (uint32_t Index = 0u; Index < AiNode->mNumMeshes; ++Index)
		{
			const auto MeshIndex = AiNode->mMeshes[Index];
			const auto Mesh = AiScene->mMeshes[MeshIndex];

			if (!Mesh)
			{
				LOG_ERROR("AssimpSceneImporter: Detected invalid mesh!");
				continue;
			}
			if (!Mesh->HasPositions())
			{
				LOG_ERROR("AssimpSceneImporter: The mesh contains no vertices data!");
				continue;
			}
			if (Mesh->mPrimitiveTypes != aiPrimitiveType_TRIANGLE)
			{
				LOG_ERROR("AssimpSceneImporter: Detected others primitive type, this should never be happen!");
				continue;
			}

			if (!Mesh->HasFaces())
			{
				LOG_ERROR("AssimpSceneImporter: The mesh constains no indices data!");
				continue;
			}

			AssimpScene.Graph.AddChild(GraphNode, Mesh->mName.C_Str());

			Math::AABB BoundingBox = Math::AABB(
				Math::Vector3(Mesh->mAABB.mMin.x, Mesh->mAABB.mMin.y, Mesh->mAABB.mMin.z),
				Math::Vector3(Mesh->mAABB.mMax.x, Mesh->mAABB.mMax.y, Mesh->mAABB.mMax.z));

			for (uint32_t FaceIndex = 0u; FaceIndex < Mesh->mNumFaces; ++FaceIndex)
			{
				const auto& Face = Mesh->mFaces[FaceIndex];
				assert(Face.mNumIndices == 3u);
				//Layout.SetFace(FaceIndex, Face.mIndices[0], Face.mIndices[1], Face.mIndices[2]);
			}

			for (uint32_t VIndex = 0u; VIndex < Mesh->mNumVertices; ++VIndex)
			{
				const auto& Position = Mesh->mVertices[VIndex];
				//Layout.SetPosition(VertexIndex, Vector3(Position.x, Position.y, Position.z));
				if (Mesh->HasNormals())
				{
					const auto& Normal = Mesh->mNormals[VIndex];
					//Layout.SetNormal(VertexIndex, Vector3(Normal.x, Normal.y, Normal.z));
				}
				if (Mesh->HasTangentsAndBitangents())
				{
					const auto& Tangent = Mesh->mTangents[VIndex];
					//Layout.SetTangent(VertexIndex, Vector3(Tangent.x, Tangent.y, Tangent.z));

					const auto& Bitangent = Mesh->mBitangents[VIndex];
					//Layout.SetBitangent(VertexIndex, Vector3(Bitangent.x, Bitangent.y, Bitangent.z));
				}

				if (Mesh->HasTextureCoords(0))
				{
					const auto& UV0 = Mesh->mTextureCoords[0u][VIndex];
				}
				if (Mesh->HasTextureCoords(1))
				{
					const auto& UV1 = Mesh->mTextureCoords[0u][VIndex];
				}

				if (Mesh->HasVertexColors(0))
				{
					const auto& Color = Mesh->mColors[0u][VIndex];
				}
			}
		}
	}
};
