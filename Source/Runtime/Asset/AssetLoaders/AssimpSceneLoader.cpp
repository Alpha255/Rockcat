#include "Asset/AssetLoaders/AssimpSceneLoader.h"
#include "Scene/Scene.h"
#include "Services/AssetDatabase.h"
#include "Paths.h"
#include "RHI/RHIDevice.h"
#include "Profile/CpuTimer.h"

#include "Scene/Components/StaticMesh.h"
#include "Scene/Components/TransformComponent.h"
#include "Scene/Components/StaticMeshComponent.h"
#include "Asset/Material.h"

#pragma warning(push)
#pragma warning(disable:4819)
#include <assimp/version.h>
#include <assimp/Importer.hpp>
#include <assimp/ProgressHandler.hpp>
#include <assimp/scene.h>
#include <assimp/GltfMaterial.h>
#include <assimp/postprocess.h>
#include <assimp/DefaultLogger.hpp>
#pragma warning(pop)

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
			LOG_DEBUG_CAT(LogAsset, "Loading assimp scene \"{}\" completed", m_AssetPath.string());
			return true;
		}

		if (static_cast<int32_t>(Percentage * 100) % 10 == 0)
		{
			LOG_DEBUG_CAT(LogAsset, "Loading assimp scene: \"{}\" in progress {:.2f}%", m_AssetPath.string(), Percentage * 100);
		}
		return false;
	}
private:
	const std::filesystem::path& m_AssetPath;
};

class AssimpLogger : public Assimp::Logger
{
public:
	void OnDebug(const char* Message) override final { LOG_DEBUG_CAT(LogAsset, Message); }
	void OnVerboseDebug(const char* Message) override final { LOG_DEBUG_CAT(LogAsset, Message); }
	void OnInfo(const char* Message) override final { LOG_INFO_CAT(LogAsset, Message); }
	void OnWarn(const char* Message) override final { LOG_WARNING_CAT(LogAsset, Message); }
	void OnError(const char* Message) override final { LOG_ERROR_CAT(LogAsset, Message); }
	bool attachStream(Assimp::LogStream*, uint32_t) override final { return true; }
	bool detachStream(Assimp::LogStream*, uint32_t) override final { return true; }
};

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

AssimpSceneLoader::AssimpSceneLoader()
	: AssetLoader({
		".3ds",
		".blend",
		".dae",
		".fbx",
		".gltf",
		".obj",
		".ply",
		".stl",
		".x"
	})
{
	LOG_INFO_CAT(LogAsset, "Create assimp scene loader, use assimp @{}.{}.{}", aiGetVersionMajor(), aiGetVersionMinor(), aiGetVersionPatch());
}

std::shared_ptr<Asset> AssimpSceneLoader::CreateAsset(const std::filesystem::path& AssetPath)
{
	return std::make_shared<AssimpScene>(AssetPath);
}

bool AssimpSceneLoader::Load(Asset& Target)
{
#if _DEBUG
	CpuTimer Timer;
#endif

	auto& Model = Cast<AssimpScene>(Target);

	const uint32_t ProcessFlags = static_cast<uint32_t>(
		aiProcessPreset_TargetRealtime_MaxQuality |
		aiProcess_ConvertToLeftHanded |  /// Use DirectX's left-hand coordinate system
		aiProcess_GenBoundingBoxes |
		aiProcess_TransformUVCoords |
		aiProcess_RemoveComponent);

	Assimp::Importer AssimpImporter;

	int32_t RemoveFlags = aiComponent::aiComponent_CAMERAS | aiComponent::aiComponent_LIGHTS;
	AssimpImporter.SetPropertyInteger(AI_CONFIG_PP_RVC_FLAGS, RemoveFlags);

#if _DEBUG
	Assimp::DefaultLogger::set(new AssimpLogger());
	AssimpImporter.SetProgressHandler(new AssimpProgressHandler(Model.GetPath()));
#endif

	if (auto AiScene = AssimpImporter.ReadFile(Model.GetPath().string(), ProcessFlags))
	{
		if (AiScene->HasMeshes() && AiScene->mRootNode)
		{
			if (ProcessNode(AiScene, AiScene->mRootNode, nullptr, Model))
			{
#if _DEBUG
				LOG_DEBUG_CAT(LogAsset, "Load assimp scene \"{}\" takes {:.2f} ms", Model.GetName(), Timer.GetElapsedMilliseconds());
#endif
				return true;
			}

			return false;
		}
		else
		{
			LOG_WARNING_CAT(LogAsset, "Assimp scene: \"{}\" has no meshes");
			return true;
		}
	}

	LOG_ERROR_CAT(LogAsset, "Failed to load assimp scene: {}: \"{}\"", Model.GetName(), AssimpImporter.GetErrorString());
	return false;
}

void AssimpSceneLoader::ProcessTransform(const aiNode* AiNode, TransformComponent& TransformComp)
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

	TransformComp.SetTranslation(Translation.x, Translation.y, Translation.z)
		.SetScale(Scalling.x, Scalling.y, Scalling.z)
		.SetRotation(Rotation.x, Rotation.y, Rotation.z, Rotation.w);
}

bool AssimpSceneLoader::ProcessNode(const aiScene* AiScene, const aiNode* AiNode, const Entity* Parent, AssimpScene& Model)
{
	if (!AiNode)
	{
		return false;
	}

	static auto SetNodeName = [](Entity& Node) {
		if (Node.GetName().Get().empty())
		{
			Node.SetName(String::Format("node%d", Node.GetID().GetIndex()));
		}
	};

	auto& ParentNode = Model.AddEntity(Parent ? Parent->GetID() : EntityID(), AiNode->mName.C_Str());

	if (AiNode == AiScene->mRootNode)
	{
		Model.SetRoot(ParentNode.GetID());
	}

	for (uint32_t Index = 0u; Index < AiNode->mNumMeshes; ++Index)
	{
		const auto MeshIndex = AiNode->mMeshes[Index];
		const auto AiMesh = AiScene->mMeshes[MeshIndex];

		if (AiMesh->HasBones())
		{
			LOG_ERROR_CAT(LogAsset, "Not supported skeletal mesh yet!");
			continue;
		}

		auto& ChildNode = Model.AddChild(ParentNode, AiMesh->mName.C_Str());
		SetNodeName(ChildNode);

		auto TransformComp = ChildNode.AddComponent<TransformComponent>();
		auto StaticMeshComp = ChildNode.AddComponent<StaticMeshComponent>();

		ProcessTransform(AiNode, *TransformComp);
		ProcessMesh(AiScene, MeshIndex, *StaticMeshComp);
		ProcessMaterial(AiScene, Model, AiMesh->mMaterialIndex, *StaticMeshComp);
	}

	if (AiNode->mNumChildren == 0u)
	{
		return true;
	}

	for (uint32_t NodeIndex = 0u; NodeIndex < AiNode->mNumChildren; ++NodeIndex)
	{
		if (!ProcessNode(AiScene, AiNode->mChildren[NodeIndex], &ParentNode, Model))
		{
			return false;
		}
	}

	return true;
}

void AssimpSceneLoader::ProcessMaterial(const aiScene* AiScene, const AssimpScene& Model, uint32_t MaterialIndex, StaticMeshComponent& StaticMeshComp)
{
	if (!AiScene->HasMaterials())
	{
		return;
	}

	if (auto AiMaterial = AiScene->mMaterials[MaterialIndex])
	{
		aiString Name;
		AiMaterial->Get(AI_MATKEY_NAME, Name);

		auto MaterialFilePath = (Paths::MaterialPath() / Model.GetStem() / Name.C_Str()).replace_extension(MaterialProperty::GetExtension());
		const bool MaterialFileExists = std::filesystem::exists(MaterialFilePath);
		auto Property = MaterialProperty::Load(MaterialFilePath);
		StaticMeshComp.SetMaterialProperty(Property);

		if (MaterialFileExists)
		{
			return;
		}

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

		ProcessTextures(AiMaterial, *Property, Model.GetPath().parent_path());

		Property->Save(true, MaterialFilePath);
	}
}

void AssimpSceneLoader::ProcessMesh(const aiScene* AiScene, uint32_t MeshIndex, StaticMeshComponent& StaticMeshComp)
{
	const auto AiMesh = AiScene->mMeshes[MeshIndex];

	if (!AiMesh)
	{
		LOG_ERROR_CAT(LogAsset, "Detected invalid mesh!");
		return;
	}
	if (!AiMesh->HasPositions())
	{
		LOG_ERROR_CAT(LogAsset, "The mesh \"{}\" has no vertices data!", AiMesh->mName.C_Str());
		return;
	}
	if (!AiMesh->HasNormals())
	{
		LOG_WARNING_CAT(LogAsset, "The mesh \"{}\" has no normals!", AiMesh->mName.C_Str());
		return;
	}
	if (!AiMesh->HasFaces())
	{
		LOG_ERROR_CAT(LogAsset, "The mesh \"{}\" has no indices data!", AiMesh->mName.C_Str());
		return;
	}
	if (AiMesh->mPrimitiveTypes != aiPrimitiveType_TRIANGLE)
	{
		LOG_ERROR_CAT(LogAsset, "The mesh \"{}\" has unsupported primitive type!", AiMesh->mName.C_Str());
		return;
	}

	auto Center = (AiMesh->mAABB.mMax + AiMesh->mAABB.mMin) * 0.5f;
	auto Extents = (AiMesh->mAABB.mMax - AiMesh->mAABB.mMin) * 0.5f;
	auto Radius = std::max<ai_real>(std::max<ai_real>(Extents.x, Extents.y), Extents.z);

	BoxSphereBounds Bounds(Math::Vector3(Center.x, Center.y, Center.z), Math::Vector3(Extents.x, Extents.y, Extents.z), Radius);
	StaticMeshComp.SetBounds(Bounds);

	auto Name = AiMesh->mName.Empty() ? String::Format("mesh%d", MeshIndex) : AiMesh->mName.C_Str();
	StaticMeshComp.SetName(std::move(Name));

	MeshData Data(
		AiMesh->mNumVertices,
		AiMesh->mNumFaces * 3u,
		AiMesh->mNumFaces,
		AiMesh->HasNormals(),
		AiMesh->HasTangentsAndBitangents(),
		AiMesh->HasTextureCoords(0u),
		AiMesh->HasTextureCoords(1u),
		AiMesh->HasVertexColors(0u),
		ERHIPrimitiveTopology::TriangleList);

	for (uint32_t FaceIndex = 0u; FaceIndex < AiMesh->mNumFaces; ++FaceIndex)
	{
		const auto& Face = AiMesh->mFaces[FaceIndex];
		assert(Face.mNumIndices == 3u);
		Data.SetFace(FaceIndex, Face.mIndices[0], Face.mIndices[1], Face.mIndices[2]);
	}

	for (uint32_t VertexIndex = 0u; VertexIndex < AiMesh->mNumVertices; ++VertexIndex)
	{
		const auto& Position = AiMesh->mVertices[VertexIndex];
		Data.SetPosition(VertexIndex, Math::Vector3(Position.x, Position.y, Position.z));

		if (AiMesh->HasNormals())
		{
			const auto& Normal = AiMesh->mNormals[VertexIndex];
			Data.SetNormal(VertexIndex, Math::Vector3(Normal.x, Normal.y, Normal.z));
		}
		if (AiMesh->HasTangentsAndBitangents())
		{
			const auto& Tangent = AiMesh->mTangents[VertexIndex];
			Data.SetTangent(VertexIndex, Math::Vector3(Tangent.x, Tangent.y, Tangent.z));

			const auto& Bitangent = AiMesh->mBitangents[VertexIndex];
			Data.SetBitangent(VertexIndex, Math::Vector3(Bitangent.x, Bitangent.y, Bitangent.z));
		}

		if (AiMesh->HasTextureCoords(0u))
		{
			const auto& UV = AiMesh->mTextureCoords[0u][VertexIndex];
			Data.SetUV0(VertexIndex, Math::Vector3(UV.x, UV.y, UV.z));
		}
		if (AiMesh->HasTextureCoords(1))
		{
			const auto& UV = AiMesh->mTextureCoords[1u][VertexIndex];
			Data.SetUV0(VertexIndex, Math::Vector3(UV.x, UV.y, UV.z));
		}

		if (AiMesh->HasVertexColors(0u))
		{
			const auto& Color = AiMesh->mColors[0u][VertexIndex];
			Data.SetColor(VertexIndex, Math::Color(Color.r, Color.g, Color.b, Color.a));
		}
	}

	if (AiMesh->HasBones())
	{
		assert(false);
	}
	else
	{
		auto Mesh = std::make_shared<StaticMesh>(Data);
		StaticMeshComp.SetMesh(Mesh);
	}
}

void AssimpSceneLoader::ProcessTextures(const aiMaterial* AiMaterial, MaterialProperty& Material, const std::filesystem::path& RootPath)
{
	std::unordered_map<std::filesystem::path, std::shared_ptr<Texture>> Textures;

	for (uint32_t Index = aiTextureType_DIFFUSE; Index < aiTextureType_TRANSMISSION; ++Index)
	{
		auto AiType = static_cast<aiTextureType>(Index);
		if (AiMaterial->GetTextureCount(AiType) == 0u || AiType == aiTextureType_UNKNOWN)
		{
			continue;
		}

		aiString URL;
		if (AI_SUCCESS != AiMaterial->GetTexture(AiType, 0u, &URL))  /// TODO: How to do if the material has more than one textures for some texture type ???
		{
			continue;
		}

		std::filesystem::path Path = RootPath / URL.C_Str();
		if (!std::filesystem::exists(Path))
		{
			continue;
		}
		
		auto Type = GetTextureType(AiType);
		if (Type != MaterialProperty::ETextureType::Num)
		{
			auto It = Textures.find(Path);
			if (It == Textures.end())
			{
				Material.Textures[Type].reset(new Texture(Path));
				Textures.emplace(Path, Material.Textures[Type]).first;
			}
			else
			{
				Material.Textures[Type] = It->second;
			}
		}
	}
}
