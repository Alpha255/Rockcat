#include "Tickable.h"

ITickable::ITickable()
{
	TickManager::Get().AddTickableObject(this);
}

void TickManager::TickObjects(float ElapsedSeconds)
{
	for (auto& Tickable : m_TickableObjets)
	{
		if (!Tickable)
		{
			continue;
		}

		auto const TickType = Tickable->GetTickType();
		const bool CanbeTick = TickType == ITickable::ETickType::Always || (TickType == ITickable::ETickType::Conditional && Tickable->IsTickable());

		if (CanbeTick)
		{
			Tickable->Tick(ElapsedSeconds);
		}
	}
}

