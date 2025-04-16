#pragma once

#include "Core/Singleton.h"

class ITickable
{
public:
	enum ETickType : uint8_t
	{
		Conditional,
		Always,
		Never
	};

	ITickable();

	virtual ~ITickable() = default;

	virtual void Tick(float ElapsedSeconds) = 0;

	virtual bool IsTickable() const { return true; }

	ETickType GetTickType() const { return m_TickType; }
	void SetTickType(ETickType TickType) { m_TickType = TickType; }
protected:
	ETickType m_TickType = ETickType::Always;
private:
};

class TickManager : public Singleton<TickManager>
{
public:
	void AddTickableObject(ITickable* Tickable)
	{
		assert(Tickable);
		m_TickableObjets.emplace(Tickable);
	}

	void RemoveTickableObject(ITickable* Tickable)
	{
		assert(Tickable);
		m_TickableObjets.erase(Tickable);
	}

	void TickObjects(float ElapsedSeconds);

	void EnqueueAsyncTickableObject(ITickable*);
private:
	std::unordered_set<ITickable*> m_TickableObjets;
};

