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
		return InEntity.AddComponent(m_ComponentPool->Allocate<T>());
	}

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
	std::vector<std::string> m_AssimpScenes;
	ComponentPool m_ComponentPool;
};

