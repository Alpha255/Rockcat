#pragma once

#include "Asset/SceneAsset.h"
#include "Components/ComponentPool.h"
#include "Core/Tickable.h"

class Scene : public ITickable, public SceneGraph, public Serializable<Scene>
{
public:
	using BaseClass::BaseClass;

	~Scene();

	void Tick(float ElapsedSeconds) override final;

	template<class T>
	T* AddComponent(Entity& InEntity)
	{
		return InEntity.AddComponent(m_ComponentPool.Allocate<T>());
	}

	const std::vector<std::shared_ptr<class ISceneView>>& GetViews() const { return m_Views; }

	template<class T>
	std::shared_ptr<T> AddView()
	{
		auto View = std::static_pointer_cast<T>(m_Views.emplace_back(std::make_shared<T>()));
		auto Cam = m_Cameras.emplace_back(std::make_shared<class Camera>());
		View->SetCamera(Cam.get());
		return View;
	}

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_BASE(BaseClass),
			CEREAL_BASE(SceneGraph),
			CEREAL_NVP(m_AssimpScenes)
		);
	}
protected:
	void OnPostLoad() override;
private:
	void MergeWithAssimpScene(const AssimpScene& InScene);

	std::vector<std::string> m_AssimpScenes;
	std::vector<std::shared_ptr<class ISceneView>> m_Views;
	std::vector<std::shared_ptr<class Camera>> m_Cameras;
};

