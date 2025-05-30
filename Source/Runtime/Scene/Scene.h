#pragma once

#include "Asset/SceneAsset.h"
#include "Components/ComponentPool.h"
#include "Core/Tickable.h"

class Scene : public ITickable, public SceneGraph, public SerializableAsset<Scene>
{
public:
	Scene(std::filesystem::path&& SubPath)
		: BaseClass(Paths::ScenePath() / SubPath)
	{
	}

	~Scene();

	static const char* GetExtension() { return ".scene"; }

	void Tick(float ElapsedSeconds) override final;

	template<class T>
	T* AddComponent(Entity& InEntity)
	{
		return InEntity.AddComponent(m_ComponentPool.Allocate<T>());
	}

	const std::vector<std::shared_ptr<class ISceneView>>& GetViews() const { return m_Views; }

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_BASE(SceneGraph),
			CEREAL_NVP(m_AssimpScenes),
			CEREAL_NVP(m_ComponentPool)
		);
	}
protected:
	void PostLoad() override;
private:
	void MergeFromAssimpScene(const AssimpSceneAsset& AssimpScene);

	std::vector<std::string> m_AssimpScenes;
	std::vector<std::shared_ptr<class ISceneView>> m_Views;
	ComponentPool m_ComponentPool;
};

