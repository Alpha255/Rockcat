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

	const std::vector<std::shared_ptr<class ISceneView>>& GetViews() const { return m_Views; }

	template<class T>
	std::shared_ptr<T> AddView()
	{
		auto View = Cast<T>(m_Views.emplace_back(std::make_shared<T>()));
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
			CEREAL_NVP(m_AssimpScenes),
			//CEREAL_NVP(m_Views),
			CEREAL_NVP(m_Cameras),
			CEREAL_NVP(m_SavedComponents)
		);
	}
protected:
	void SetStatusChangeCallbacks() override final;

	inline const std::vector<EntityID::IndexType>& GetAddedPrimitives() const { return m_AddedPrimitives; }
	inline const std::vector<EntityID::IndexType>& GetRemovedPrimitives() const { return m_RemovedPrimitives; }
private:
	void MergeAssimpScenes(std::vector<std::shared_ptr<AssimpScene>>* AssimpScenes);

	inline void AddPrimitive(const Entity& InEntity)
	{
		m_AddedPrimitives.push_back(InEntity.GetID().GetIndex());
	}

	inline void RemovePrimitive(const Entity& InEntity)
	{
		m_RemovedPrimitives.push_back(InEntity.GetID().GetIndex());
	}

	std::vector<std::string> m_AssimpScenes;
	std::vector<std::shared_ptr<class ISceneView>> m_Views;
	std::vector<std::shared_ptr<class Camera>> m_Cameras;

	std::vector<EntityID::IndexType> m_AddedPrimitives;
	std::vector<EntityID::IndexType> m_RemovedPrimitives;

	std::unordered_map<size_t, std::vector<std::shared_ptr<ComponentBase>>> m_SavedComponents;
};

