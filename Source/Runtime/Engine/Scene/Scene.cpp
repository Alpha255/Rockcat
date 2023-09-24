#if 0
#include "Runtime/Scene/Scene.h"

Scene::Scene(const char8_t* Path)
	: Serializeable(IAsset::CatPath(ASSET_PATH_SCENES, Path))
{
}

void Scene::OnLoadCompletion()
{
	auto PreLoad = [](
		auto& Src, 
		std::unordered_map<SceneNode::ObjectIDType, std::shared_ptr<SceneNode>>& Dst) {
		for (auto& KeyValue : Src)
		{
			auto& ID = KeyValue.first;
			auto& Object = KeyValue.second;

			//SceneNode::IDPool->Reserve(ID);

			Dst.insert(std::make_pair(ID, Object));
		}
	};

	PreLoad(m_DirectionalLights, m_Objects);
	PreLoad(m_PointLights, m_Objects);
	PreLoad(m_SpotLights, m_Objects);
	PreLoad(m_AreaLights, m_Objects);
	PreLoad(m_Models, m_Objects);
	PreLoad(m_Cameras, m_Objects);
	PreLoad(m_DefaultObjects, m_Objects);

	for (auto& KeyValue : m_Objects)
	{
		auto& Object = KeyValue.second;

		for (auto& Child : Object->m_ChildrenIDs)
		{
			Object->m_Children.emplace_back(std::weak_ptr<SceneNode>(m_Objects[Child]));
		}
	}

	if (m_RootID.IsValid())
	{
		m_Root = m_Objects[m_RootID].get();
	}
	else
	{
		auto Root = std::make_shared<SceneNode>(SceneNode::EType::Default, "Root");
		SetRoot(Root);
		m_DefaultObjects.insert(std::make_pair(Root->ID(), Root));
	}

	if (m_Cameras.size() == 0u)
	{
		AddCamera(Camera::EMode::FirstPerson, nullptr);
	}
	else
	{
		for (auto& Camera : m_Cameras)
		{
			m_CameraList.emplace_back(Camera.second.get());
		}
	}

	for (auto& KeyValue : m_Models)
	{
		KeyValue.second->Reimport(KeyValue.second->m_Model->Path.c_str());
		/// #TODO: Need to transform children meshes
	}
}

void Scene::AddModel(const std::shared_ptr<ModelInstance>& Model, const std::shared_ptr<SceneNode>& Parent)
{
	if (Model)
	{
		assert(Model->Type() == SceneNode::EType::Model);

		if (AddObject(Model, Parent))
		{
			m_Models.insert(std::make_pair(Model->ID(), Model));
		}
	}
}

void Scene::AddLight(const std::shared_ptr<ILight>& Light, const std::shared_ptr<SceneNode>& Parent)
{
	if (Light)
	{
		assert(Light->Type() == SceneNode::EType::Light);

		if (AddObject(Light, Parent))
		{
			m_Lights.emplace_back(Light.get());

			switch (Light->LightType())
			{
			case ELightType::Directional:
				m_DirectionalLights.insert(std::make_pair(Light->ID(), std::static_pointer_cast<DirectionalLight>(Light)));
				break;
			case ELightType::Point:
				m_PointLights.insert(std::make_pair(Light->ID(), std::static_pointer_cast<PointLight>(Light)));
				break;
			case ELightType::Spot:
				m_SpotLights.insert(std::make_pair(Light->ID(), std::static_pointer_cast<SpotLight>(Light)));
				break;
			case ELightType::Area:
				m_AreaLights.insert(std::make_pair(Light->ID(), std::static_pointer_cast<AreaLight>(Light)));
				break;
			case ELightType::Unknown:
				assert(0);
				break;
			}
		}
	}
}

std::shared_ptr<Camera> Scene::AddCamera(Camera::EMode Mode, const std::shared_ptr<SceneNode>& Parent)
{
	auto TempCamera = std::make_shared<Camera>(Mode);

	AddCamera(TempCamera, Parent);

	return TempCamera;
}

void Scene::AddCamera(const std::shared_ptr<Camera>& Cam, const std::shared_ptr<SceneNode>& Parent)
{
	if (Cam)
	{
		assert(Cam->Type() == SceneNode::EType::Camera);

		if (AddObject(Cam, Parent))
		{
			m_CameraList.emplace_back(Cam.get());
			m_Cameras.insert(std::make_pair(Cam->ID(), Cam));
		}
	}
}

bool8_t Scene::AddObject(const std::shared_ptr<SceneNode>& Object, const std::shared_ptr<SceneNode>& Parent)
{
	if (Object)
	{
		assert(Object->ID().IsValid());

		if (m_Objects.find(Object->ID()) != m_Objects.end())
		{
			return false;
		}

		m_Objects.insert(std::make_pair(Object->ID(), Object));

		if (Parent)
		{
			Parent->AddChild(Object);
		}
		else
		{
			assert(m_Root);
			m_Root->AddChild(Object);
		}

		if (Object->Tickable())
		{
			m_TickableObjects.push_back(Object.get());
		}

		if (Object->Type() == SceneNode::EType::Default)
		{
			m_DefaultObjects.insert(std::make_pair(Object->ID(), Object));
		}

		SetDirty();

		return true;
	}

	return false;
}

void Scene::SetRoot(std::shared_ptr<SceneNode>& Root)
{
	if (Root)
	{
		if (m_Root)
		{
			if (m_Root->ID() != Root->ID())
			{
				for (auto Child : m_Root->m_ChildrenIDs)
				{
					Root->AddChild(m_Objects[Child]);
				}

				m_Root->m_ChildrenIDs.clear();
				m_Root = Root.get();
				m_RootID = Root->ID();

				if (m_Objects.find(Root->ID()) == m_Objects.end())
				{
					m_Objects.insert(std::make_pair(Root->ID(), Root));
				}
			}
		}
		else
		{
			assert(Root->ID().IsValid());

			m_Root = Root.get();
			m_RootID = Root->ID();
			m_Objects.insert(std::make_pair(Root->ID(), Root));
		}

		SetDirty();
	}
}

std::shared_ptr<ModelInstance> Scene::AddModel(const char8_t* ModelPath, const std::shared_ptr<SceneNode>& Parent)
{
	auto Model = std::make_shared<ModelInstance>(ModelPath);
	
	AddModel(Model, Parent);

	return Model;
}

void SceneNode::Traverse(const VisitFunc& Visit) const
{
	if (Visit)
	{
		Visit(this);

		for (auto& Child : m_Children)
		{
			if (!Child.expired())
			{
				auto Temp = Child.lock();

				Temp->Traverse(Visit);

				/// #TODO: Handle circular reference???
			}
		}
	}
}

void Scene::Tick(float32_t ElapsedSeconds)
{
	for (auto Object : m_TickableObjects)
	{
		if (!Object->Enabled())
		{
			continue;
		}

		if (!Object->Visible())
		{
			continue;
		}

		Object->Tick(ElapsedSeconds);
	}
}

void Scene::Traverse(const SceneNode::VisitFunc& Visit) const
{
	if (Visit && m_Root)
	{
		m_Root->Traverse(Visit);
	}
}
#endif

#include "Runtime/Engine/Scene/Scene.h"
