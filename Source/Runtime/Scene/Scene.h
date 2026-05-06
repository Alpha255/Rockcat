#pragma once

#include "Asset/SerializableAsset.h"
#include "Scene/SceneGraph.h"
#include "Components/ComponentPool.h"
#include "Components/Camera.h"
#include "Core/Tickable.h"

struct AssimpScene : public Asset, public SceneGraph
{
	using Asset::Asset;
};

class Scene : public ITickable, public SceneGraph, public Serializable<Scene>
{
public:
	using BaseClass::BaseClass;

	~Scene();

	void Tick(float ElapsedSeconds) override final;

	void AddPrimitive(const class PrimitiveComponent* PrimitiveComp);
	void RemovePrimitive(const class PrimitiveComponent* PrimitiveComp);

	inline const std::vector<const class PrimitiveComponent*>& GetAddedPrimitives() const { return m_AddedPrimitives; }
	inline const std::vector<const class PrimitiveComponent*>& GetRemovedPrimitives() const { return m_RemovedPrimitives; }

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_BASE(BaseClass),
			CEREAL_BASE(SceneGraph),
			CEREAL_NVP(m_AssimpScenes),
			CEREAL_NVP(m_Cameras),
			CEREAL_NVP(m_Components)
		);
	}
protected:
	void OnPostLoad() override final;
private:
	std::vector<std::string> m_AssimpScenes;

	std::vector<std::shared_ptr<Camera>> m_Cameras;

	std::vector<const class PrimitiveComponent*> m_AddedPrimitives;
	std::vector<const class PrimitiveComponent*> m_RemovedPrimitives;

	std::unordered_map<size_t, std::vector<std::shared_ptr<ComponentBase>>> m_Components;

	std::shared_ptr<AssetLoadRequests> m_AssimpLoadRequests;
};

