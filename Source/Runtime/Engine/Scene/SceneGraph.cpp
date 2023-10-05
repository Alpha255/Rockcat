#include "Runtime/Engine/Scene/SceneGraph.h"

SceneGraph::Node& SceneGraph::Node::SetMasks(ENodeMasks Masks) 
{ 
	m_Masks = (Masks | m_Masks); return *this; 
}

bool8_t SceneGraph::Node::IsStaticMesh() const 
{ 
	return (m_Masks & ENodeMasks::StaticMesh) == ENodeMasks::StaticMesh; 
}

bool8_t SceneGraph::Node::IsSkeletalMesh() const 
{ 
	return (m_Masks & ENodeMasks::SkeletalMesh) == ENodeMasks::SkeletalMesh; 
}

bool8_t SceneGraph::Node::IsLight() const 
{ 
	return (m_Masks & ENodeMasks::Light) == ENodeMasks::Light; 
}

bool8_t SceneGraph::Node::IsCamera() const 
{ 
	return (m_Masks & ENodeMasks::Camera) == ENodeMasks::Camera; 
}