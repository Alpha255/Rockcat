#pragma once

#include "Engine/Asset/SceneAsset.h"
#include "Engine/Asset/AssetDatabase.h"
#include "Engine/Asset/GlobalShaders/DefaultShading.h"
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
	}

	void LoadAssetData(std::shared_ptr<Asset>&, AssetType::EContentsType) override final {} /// Just load from file to avoid base path of texture broken

	std::shared_ptr<Asset> CreateAsset(const std::filesystem::path& AssetPath) override final { return std::make_shared<AssimpSceneAsset>(AssetPath); }

	bool Reimport(Asset& InAsset) override final
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
						CompileMaterials(AssimpScene);
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

			auto GraphNodeID = AssimpScene.Graph.AddChild(GraphNode, Mesh->mName.C_Str(), SceneGraph::Node::ENodeMasks::StaticMesh);

			if (Mesh->HasBones())
			{
				assert(false);
			}
			else
			{
				AssimpScene.Graph.GetNode(GraphNodeID).SetDataIndex(MeshIndex);
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

		AssimpScene.Data.Materials.resize(AiScene->mNumMaterials);
		for (uint32_t Index = 0u; Index < AiScene->mNumMaterials; ++Index)
		{
			if (auto AiMaterial = AiScene->mMaterials[Index])
			{
				aiString Name;
				AiMaterial->Get(AI_MATKEY_NAME, Name);

				aiString AlphaMode;
				AiMaterial->Get(AI_MATKEY_GLTF_ALPHAMODE, AlphaMode);

				aiShadingMode ShadingMode = aiShadingMode_Unlit;
				AiMaterial->Get(AI_MATKEY_SHADING_MODEL, ShadingMode);

				aiColor4D BaseColor(1.0f, 1.0f, 1.0f, 1.0f);
				AiMaterial->Get(AI_MATKEY_BASE_COLOR, BaseColor);

				aiColor4D DiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
				AiMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, DiffuseColor);

				aiColor4D SpecularColor(1.0f, 1.0f, 1.0f, 1.0f);
				AiMaterial->Get(AI_MATKEY_COLOR_SPECULAR, SpecularColor);

				aiColor4D EmissiveColor(1.0f, 1.0f, 1.0f, 1.0f);
				AiMaterial->Get(AI_MATKEY_COLOR_EMISSIVE, EmissiveColor);

				aiColor4D TransparentColor(1.0f, 1.0f, 1.0f, 1.0f);
				AiMaterial->Get(AI_MATKEY_COLOR_TRANSPARENT, TransparentColor);

				aiColor4D ReflectiveColor(1.0f, 1.0f, 1.0f, 1.0f);
				AiMaterial->Get(AI_MATKEY_COLOR_REFLECTIVE, ReflectiveColor);

				ai_real MetallicFactor = 1.0f;
				AiMaterial->Get(AI_MATKEY_METALLIC_FACTOR, MetallicFactor);

				ai_real RoughnessFactor = 1.0f;
				AiMaterial->Get(AI_MATKEY_ROUGHNESS_FACTOR, RoughnessFactor);

				ai_real GlossinessFactor = 1.0f;
				AiMaterial->Get(AI_MATKEY_GLOSSINESS_FACTOR, GlossinessFactor);

				ai_real SpecularFactor = 1.0f;
				AiMaterial->Get(AI_MATKEY_SPECULAR_FACTOR, SpecularFactor);

				ai_real Opacity = 1.0f;
				AiMaterial->Get(AI_MATKEY_OPACITY, Opacity);

				ai_real Shininess = 1.0f;
				AiMaterial->Get(AI_MATKEY_SHININESS, Shininess);

				ai_real AlphaCutoff = 0.0f;
				AiMaterial->Get(AI_MATKEY_GLTF_ALPHACUTOFF, AlphaCutoff);

				bool TwoSided = false;
				AiMaterial->Get(AI_MATKEY_TWOSIDED, TwoSided);

				auto MaterialAssetName = (AssimpScene.GetPath().stem() / Name.C_Str()).string();
				auto& Material = AssimpScene.Data.Materials.at(Index);

				switch (ShadingMode)
				{
				case aiShadingMode_Flat:
				case aiShadingMode_Gouraud:
				case aiShadingMode_Phong:
				case aiShadingMode_Blinn:
					Material = MaterialAsset::Load<MaterialLit>(Index, EShadingMode::BlinnPhong, MaterialAssetName.c_str());
					break;
				case aiShadingMode_Toon:
					Material = MaterialAsset::Load<MaterialToon>(Index, MaterialAssetName.c_str());
					break;
				case aiShadingMode_OrenNayar:
				case aiShadingMode_Minnaert:
				case aiShadingMode_CookTorrance:
				case aiShadingMode_Fresnel:
				case aiShadingMode_PBR_BRDF:
					Material = MaterialAsset::Load<MaterialLit>(Index, EShadingMode::StandardPBR, MaterialAssetName.c_str());
					break;
				case aiShadingMode_NoShading:
					Material = MaterialAsset::Load<MaterialUnlit>(Index, MaterialAssetName.c_str());
					break;
				}

				ProcessTextures(AiMaterial, Material.get(), AssimpScene.GetPath().parent_path());
			}
			else
			{
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
				LOG_CAT_ERROR(LogAssimpImporter, "The mesh has no normals!");
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

			if (Mesh->HasTangentsAndBitangents())
			{
				GetVertexShader(AssimpScene, Mesh->mMaterialIndex).SetDefine("_HAS_TANGENT_", true);
				GetFragmentShader(AssimpScene, Mesh->mMaterialIndex).SetDefine("_HAS_TANGENT_", true);
			}
			if (Mesh->HasTextureCoords(0u))
			{
				GetVertexShader(AssimpScene, Mesh->mMaterialIndex).SetDefine("_HAS_UV0_", true);
			}
			if (Mesh->HasTextureCoords(1u))
			{
				GetVertexShader(AssimpScene, Mesh->mMaterialIndex).SetDefine("_HAS_UV1_", true);
			}
			if (Mesh->HasVertexColors(0u))
			{
				GetVertexShader(AssimpScene, Mesh->mMaterialIndex).SetDefine("_HAS_COLOR_", true);
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
				AssimpScene.Data.StaticMeshes.emplace_back(std::make_shared<StaticMesh>(MeshDataBlock, Mesh->mMaterialIndex));
			}
		}
	}

	std::shared_ptr<Asset> SetMaterialUnlitTexture(MaterialAsset* Material, aiTextureType TextureType, const std::filesystem::path& TexturePath)
	{
		std::shared_ptr<TextureAsset> Image(nullptr);

		if (Material->GetShadingMode() == EShadingMode::Unlit)
		{
			auto Unlit = Cast<MaterialUnlit>(Material);

			switch (TextureType)
			{
			case aiTextureType_DIFFUSE:
			case aiTextureType_BASE_COLOR:
				Unlit->SetBaseColorMap(TexturePath);
				//Image = Unlit->GetBaseColorMap();
				break;
			}
		}

		return Image;
	}

	std::shared_ptr<Asset> SetMaterialLitTexture(MaterialAsset* Material, aiTextureType TextureType, const std::filesystem::path& TexturePath)
	{
		std::shared_ptr<TextureAsset> Image(nullptr);

		if (Material->GetShadingMode() == EShadingMode::BlinnPhong || Material->GetShadingMode() == EShadingMode::StandardPBR)
		{
			auto Lit = Cast<MaterialLit>(Material);

			switch (TextureType)
			{
			case aiTextureType_DIFFUSE:
			case aiTextureType_BASE_COLOR:
				Lit->SetBaseColorMap(TexturePath);
				//Image = Lit->DefaultLitFS::GetShaderVariables().GetBaseColorMap();
				break;
			case aiTextureType_NORMALS:
				Lit->SetNormalMap(TexturePath);
				//Image = Lit->DefaultLitFS::GetShaderVariables().GetNormalMap();
				break;
			case aiTextureType_REFLECTION:
				break;
			case aiTextureType_EMISSIVE:
			case aiTextureType_EMISSION_COLOR:
				break;
			}

			if (Material->GetShadingMode() == EShadingMode::BlinnPhong)
			{
				switch (TextureType)
				{
				case aiTextureType_SPECULAR:
					break;
				case aiTextureType_AMBIENT:
					break;
				}
			}
			else if (Material->GetShadingMode() == EShadingMode::StandardPBR)
			{
				switch (TextureType)
				{
				case aiTextureType_SHININESS:
					break;
				case aiTextureType_OPACITY:
					break;
				case aiTextureType_METALNESS:
					Lit->SetMetallicRoughnessMap(TexturePath);
					//Image = Lit->DefaultLitFS::GetShaderVariables().GetMetallicRoughnessMap();
					break;
				case aiTextureType_DIFFUSE_ROUGHNESS:
					break;
				case aiTextureType_AMBIENT_OCCLUSION:
					Lit->SetAOMap(TexturePath);
					//Image = Lit->DefaultLitFS::GetShaderVariables().GetAOMap();
					break;
				case aiTextureType_SHEEN:
					break;
				case aiTextureType_CLEARCOAT:
					break;
				}
			}
		}

		return Image;
	}

	std::shared_ptr<Asset> SetMaterialToonTexture(MaterialAsset* Material, aiTextureType TextureType, const std::filesystem::path& TexturePath)
	{
		std::shared_ptr<TextureAsset> Image(nullptr);

		if (Material->GetShadingMode() == EShadingMode::Toon)
		{
			auto Toon = Cast<MaterialToon>(Material);

			switch (TextureType)
			{
			case aiTextureType_DIFFUSE:
			case aiTextureType_BASE_COLOR:
				Toon->SetBaseColorMap(TexturePath);
				//Image = Toon->GetBaseColorMap();
				break;
			}
		}

		return Image;
	}

	ShaderAsset& GetVertexShader(AssimpSceneAsset& AssimpScene, uint32_t MaterialIndex)
	{
		return const_cast<ShaderAsset&>(AssimpScene.Data.Materials[MaterialIndex]->GetVertexShader());
	}

	ShaderAsset& GetFragmentShader(AssimpSceneAsset& AssimpScene, uint32_t MaterialIndex)
	{
		return const_cast<ShaderAsset&>(AssimpScene.Data.Materials[MaterialIndex]->GetFragmentShader());
	}

	void ProcessTextures(const aiMaterial* AiMaterial, MaterialAsset* Material, const std::filesystem::path& RootPath)
	{
		for (uint32_t Index = aiTextureType_DIFFUSE; Index < aiTextureType_TRANSMISSION; ++Index)
		{
			auto TextureType = static_cast<aiTextureType>(Index);
			if (AiMaterial->GetTextureCount(TextureType) == 0u || TextureType == aiTextureType_UNKNOWN)
			{
				continue;
			}

			aiString Path;
			if (AI_SUCCESS == AiMaterial->GetTexture(TextureType, 0u, &Path))  /// TODO: How to do if the material has more than one textures for some texture type ???
			{
				std::filesystem::path TexturePath = RootPath / Path.C_Str();
				if (std::filesystem::exists(TexturePath))
				{
					auto AssetLoadCallbacks = std::make_optional(Asset::Callbacks{});
					AssetLoadCallbacks.value().PreLoadCallback = [this, &TextureType](Asset& InAsset) {
						Cast<TextureAsset>(InAsset).SetUseSRGB(TextureType == aiTextureType_DIFFUSE || TextureType == aiTextureType_BASE_COLOR);
					};

					auto Image = SetMaterialUnlitTexture(Material, TextureType, TexturePath);
					if (!Image)
					{
						Image = SetMaterialLitTexture(Material, TextureType, TexturePath);
						if (!Image)
						{
							Image = SetMaterialToonTexture(Material, TextureType, TexturePath);
						}
					}

					if (Image)
					{
						AssetDatabase::Get().FindOrImportAsset(Image, AssetLoadCallbacks);
					}
				}
			}
		}
	}

	void CompileMaterials(AssimpSceneAsset& /*AssimpScene*/)
	{
		//for (auto& Material : AssimpScene.Data.Materials)
		//{
		//	Material->Compile();
		//}
	}
};
