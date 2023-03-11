#include "Runtime/SceneNode.h"

std::unique_ptr<IDPoolThreadSafe<SceneNode::ObjectID>> SceneNode::IDPool = nullptr;

SceneNode::SceneNode(EType Type, const char8_t* Name, bool8_t Tickable)
	: m_Type(Type)
	, m_Name(Name ? Name : NAMELESS_SCENE_NODE)
	, m_Tickable(Tickable)
{
	SetID(IDPool->Alloc());
}

SceneNode::SceneNode(const SceneNode& Other)
	: m_Type(Other.m_Type)
	, m_Enabled(Other.m_Enabled)
	, m_Visible(Other.m_Visible)
	, m_Selected(Other.m_Selected)
	, m_Tickable(Other.m_Tickable)
	, m_InstanceCount(Other.m_InstanceCount + 1)
	, m_Position(Other.m_Position)
	, m_Scaling(Other.m_Scaling)
	, m_Rotation(Other.m_Rotation)
	, m_Name(String::Format("%s_%d", Other.m_Name.c_str(), Other.m_InstanceCount + 1))
	, m_Parent(Other.m_Parent)
{
	SetID(IDPool->Alloc());
}

SceneNode& SceneNode::operator=(const SceneNode& Other)
{
	m_Type = Other.m_Type;
	m_Enabled = Other.m_Enabled;
	m_Visible = Other.m_Visible;
	m_Selected = Other.m_Selected;
	m_Tickable = Other.m_Tickable;
	m_InstanceCount = Other.m_InstanceCount + 1;
	m_Position = Other.m_Position;
	m_Scaling = Other.m_Scaling;
	m_Rotation = Other.m_Rotation;
	m_Name = String::Format("%s_%d", Other.m_Name.c_str(), Other.m_InstanceCount + 1);
	m_Parent = Other.m_Parent;

	return *this;
}