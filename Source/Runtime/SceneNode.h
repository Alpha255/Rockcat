#pragma once

#include "Core/Gear.h"

#define NAMELESS_SCENE_NODE "NamelessSceneNode"

class SceneNode : public ITickable, public std::enable_shared_from_this<SceneNode>
{
public:
	using ObjectID = uint32_t;
	static constexpr ObjectID InvalidID = ~0u;
	using VisitFunc = std::function<bool8_t(const SceneNode*)>;

	enum class EType : uint8_t
	{
		Default,
		Model,
		StaticMesh,
		SkinnedMesh,
		Light,
		Camera
	};

	SceneNode(EType Type, const char8_t* Name = NAMELESS_SCENE_NODE, bool8_t Tickable = false);

	SceneNode(const SceneNode&);

	SceneNode& operator=(const SceneNode&);

	virtual ~SceneNode() = default;

	const char8_t* Name() const
	{
		return m_Name.c_str();
	}

	void SetName(const char8_t* Name)
	{
		m_Name = Name;
	}

	EType Type() const
	{
		return m_Type;
	}

	ObjectID ID() const
	{
		return m_ID;
	}

	uint32_t InstanceCount() const
	{
		return m_InstanceCount;
	}

	bool8_t Enabled() const
	{
		return m_Enabled;
	}

	bool8_t Visible() const
	{
		return m_Visible;
	}

	bool8_t Selected() const
	{
		return m_Selected;
	}

	bool8_t Tickable() const
	{
		return m_Tickable;
	}

	const Vector3& Position() const
	{
		return m_Position;
	}

	void Translate(const Vector3& Translation)
	{
		m_Position += Translation;
	}

	void Translate(float32_t X, float32_t Y, float32_t Z)
	{
		m_Position += Vector3(X, Y, Z);
	}

	const Vector3& Scaling() const
	{
		return m_Scaling;
	}

	void Scale(const Vector3& Scaling)
	{
		m_Scaling *= Scaling;
	}

	void Scale(float32_t X, float32_t Y, float32_t Z)
	{
		m_Scaling *= Vector3(X, Y, Z);
	}

	const Quaternion Rotation() const
	{
		return m_Rotation;
	}

	std::shared_ptr<SceneNode> Parent() const
	{
		return m_Parent.lock();
	}

	void SetParent(const std::shared_ptr<SceneNode>& Parent)
	{
		if (Parent)
		{
			assert(Parent->ID() != InvalidID && Parent->ID() != m_ID);
			m_Parent = Parent->weak_from_this();
		}
		else
		{
			if (!m_Parent.expired())
			{
				auto Temp = m_Parent.lock();
				m_Parent.reset();

				auto TempThis = shared_from_this();
				Temp->RemoveChild(TempThis);
			}
		}
	}

	void AddChild(const std::shared_ptr<SceneNode>& Child)
	{
		if (Child)
		{
			auto Find = std::find_if(m_Children.begin(), m_Children.end(),
				[Child](const std::weak_ptr<SceneNode>& Node)
				{
					return Node.lock().get() == Child.get();
				});
			if (Find == m_Children.end())
			{
				m_Children.emplace_back(Child);
				m_ChildrenIDs.insert(Child->ID());
				Child->SetParent(shared_from_this());
			}
		}
	}

	void RemoveChild(std::shared_ptr<SceneNode>& Child)
	{
		if (Child)
		{
			auto Find = std::find_if(m_Children.begin(), m_Children.end(),
				[Child](const std::weak_ptr<SceneNode>& Node)
				{
					return Node.lock().get() == Child.get();
				});
			if (Find != m_Children.end())
			{
				Child->SetParent(nullptr);
				m_Children.erase(Find);
				m_ChildrenIDs.erase(Child->ID());
			}
		}
	}

	void Traverse(const VisitFunc& Visit) const;

	void Tick(float32_t /*ElapsedSeconds*/) override {}

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_NVP(m_Type),
			CEREAL_NVP(m_Enabled),
			CEREAL_NVP(m_Visible),
			CEREAL_NVP(m_Selected),
			CEREAL_NVP(m_Tickable),
			CEREAL_NVP(m_ID),
			CEREAL_NVP(m_Position),
			CEREAL_NVP(m_Scaling),
			CEREAL_NVP(m_Rotation),
			CEREAL_NVP(m_Name),
			CEREAL_NVP(m_ChildrenIDs)
		);
	}
protected:
	friend class Scene;
	friend class cereal::access;

	SceneNode() = default;

	void SetID(ObjectID ID)
	{
		m_ID = ID;
	}

	void SetType(EType Type)
	{
		m_Type = Type;
	}

	EType m_Type = EType::Default;
private:
	bool8_t m_Enabled = true;
	bool8_t m_Visible = true;
	bool8_t m_Selected = false;
	bool8_t m_Tickable = false;

	ObjectID m_ID = InvalidID;
	uint32_t m_InstanceCount = 1u;
	std::weak_ptr<SceneNode> m_Parent;
	std::vector<std::weak_ptr<SceneNode>> m_Children;

	Vector3 m_Position;
	Vector3 m_Scaling{ 1.0f, 1.0f, 1.0f };
	Quaternion m_Rotation;

	std::string m_Name;

	std::set<ObjectID> m_ChildrenIDs;

	static std::unique_ptr<IDPoolThreadSafe<ObjectID>> IDPool;
};
