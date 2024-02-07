#include "Engine/Scene/SceneGraph.h"

SceneGraph::Node& SceneGraph::Node::SetMasks(ENodeMasks Masks) 
{ 
	m_Masks = (Masks | m_Masks); return *this;
}

bool SceneGraph::Node::IsStaticMesh() const 
{
	return (m_Masks & ENodeMasks::StaticMesh) == ENodeMasks::StaticMesh;
}

bool SceneGraph::Node::IsSkeletalMesh() const 
{
	return (m_Masks & ENodeMasks::SkeletalMesh) == ENodeMasks::SkeletalMesh;
}

bool SceneGraph::Node::IsLight() const 
{
	return (m_Masks & ENodeMasks::Light) == ENodeMasks::Light;
}

bool SceneGraph::Node::IsCamera() const 
{
	return (m_Masks & ENodeMasks::Camera) == ENodeMasks::Camera;
}