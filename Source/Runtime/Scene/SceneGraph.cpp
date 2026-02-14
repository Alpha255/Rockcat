#include "Scene/SceneGraph.h"

void Merge(SceneGraph& Dest, const SceneGraph& Src)
{
	if (Dest.IsEmpty())
	{
		Dest.SetRoot(*Src.GetRoot());
		Dest.m_Entities.insert(Dest.m_Entities.end(), Src.m_Entities.begin(), Src.m_Entities.end());
	}
	else
	{
		const EntityID::IndexType Offset = static_cast<EntityID::IndexType>(Dest.GetNumEntity());
		auto Start = Dest.m_Entities.insert(Dest.m_Entities.end(), Src.m_Entities.begin(), Src.m_Entities.end());

		for (auto It = Start; It != Dest.m_Entities.end(); ++It)
		{
			Dest.SetEntityID(*It, EntityID(It->GetID() + Offset));
		}
	}
}

void Integrate(SceneGraph& Dest, SceneGraph& Src)
{
	if (Dest.IsEmpty())
	{
		Dest.SetRoot(*Src.GetRoot());
		Dest.m_Entities = std::move(Src.m_Entities);
	}
	else
	{
		const EntityID::IndexType Offset = static_cast<EntityID::IndexType>(Dest.GetNumEntity());
		for (auto& Node : Src.GetAllEntities())
		{
			Src.SetEntityID(Node, EntityID(Node.GetID() + Offset));
		}

		Dest.m_Entities.insert(Dest.m_Entities.end(), Src.m_Entities.begin(), Src.m_Entities.end());
	}
}
