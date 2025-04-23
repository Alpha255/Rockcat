#include "Scene/SceneGraph.h"

bool SceneGraph::Node::IsPrimitive() const 
{
	return (m_Masks & ENodeMasks::Primitive) == ENodeMasks::Primitive;
}

bool SceneGraph::Node::IsLight() const 
{
	return (m_Masks & ENodeMasks::Light) == ENodeMasks::Light;
}

bool SceneGraph::Node::IsCamera() const 
{
	return (m_Masks & ENodeMasks::Camera) == ENodeMasks::Camera;
}

SceneGraph::Node SceneGraph::RemoveNode(NodeID ID)
{
	(void)ID;
	assert(0);
	return Node();
}

void SceneGraph::PuregeDeadNodes()
{
}