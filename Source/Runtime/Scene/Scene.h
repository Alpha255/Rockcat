#pragma once

#include "Runtime/Camera/Camera.h"
#include "Runtime/Scene/Light.h"
#include "Runtime/Asset/Model.h"

class Scene : public Serializeable<Scene>, public ITickable
{
public:
	Scene(const char8_t* Path);
	
	void AddModel(const std::shared_ptr<ModelInstance>& Model, const std::shared_ptr<SceneNode>& Parent);

	void AddLight(const std::shared_ptr<ILight>& Light, const std::shared_ptr<SceneNode>& Parent);

	void AddCamera(const std::shared_ptr<Camera>& Camera, const std::shared_ptr<SceneNode>& Parent);

	std::shared_ptr<Camera> AddCamera(Camera::EMode Mode, const std::shared_ptr<SceneNode>& Parent);

	std::shared_ptr<ModelInstance> AddModel(const char8_t* ModelPath, const std::shared_ptr<SceneNode>& Parent);

	void Traverse(const SceneNode::VisitFunc& Visit) const;

	void SetMainCamera(uint32_t Index)
	{
		assert(Index < m_CameraList.size());
		m_MainCameraIndex = Index;
	}

	Camera* MainCamera() const
	{
		assert(m_MainCameraIndex < m_CameraList.size());
		return m_CameraList[m_MainCameraIndex];
	}

	const std::vector<Camera*>& Cameras() const
	{
		return m_CameraList;
	}

	void SetRoot(std::shared_ptr<SceneNode>& Root);

	void Tick(float32_t ElapsedSeconds) override final;

	SceneNode* Root() const
	{
		assert(m_Root);
		return m_Root;
	}

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_NVP(m_MainCameraIndex),
			CEREAL_NVP(m_RootID),
			CEREAL_NVP(m_DirectionalLights),
			CEREAL_NVP(m_PointLights),
			CEREAL_NVP(m_SpotLights),
			CEREAL_NVP(m_AreaLights),
			CEREAL_NVP(m_Models),
			CEREAL_NVP(m_Cameras),
			CEREAL_NVP(m_DefaultObjects)
		);
	}
protected:
	bool8_t AddObject(const std::shared_ptr<SceneNode>& Object, const std::shared_ptr<SceneNode>& Parent);
	void OnLoadCompletion() override final;
private:
	std::vector<Camera*> m_CameraList;
	std::vector<ILight*> m_Lights;

	uint32_t m_MainCameraIndex = 0u;
	SceneNode::ObjectID m_RootID;
	SceneNode* m_Root = nullptr;

	std::unordered_map<SceneNode::ObjectIDType, std::shared_ptr<SceneNode>> m_Objects;

	/// For the sake of serialization without RTTI
	std::unordered_map<SceneNode::ObjectIDType, std::shared_ptr<DirectionalLight>> m_DirectionalLights;
	std::unordered_map<SceneNode::ObjectIDType, std::shared_ptr<PointLight>> m_PointLights;
	std::unordered_map<SceneNode::ObjectIDType, std::shared_ptr<SpotLight>> m_SpotLights;
	std::unordered_map<SceneNode::ObjectIDType, std::shared_ptr<AreaLight>> m_AreaLights;
	std::unordered_map<SceneNode::ObjectIDType, std::shared_ptr<ModelInstance>> m_Models;
	std::unordered_map<SceneNode::ObjectIDType, std::shared_ptr<Camera>> m_Cameras;
	std::unordered_map<SceneNode::ObjectIDType, std::shared_ptr<SceneNode>> m_DefaultObjects;

	std::vector<SceneNode*> m_TickableObjects;
};
