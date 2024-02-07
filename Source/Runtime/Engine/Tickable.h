#pragma once

#include "Core/Definitions.h"

class ITickable
{
public:
	enum ETickType : uint8_t
	{
		Conditional,
		Always,
		Never
	};

	virtual ~ITickable() = default;

	virtual void Tick(float ElapsedSeconds) = 0;

	virtual bool IsTickable() const { return true; }

	ETickType GetTickType() const { return m_TickType; }

	void SetTickType(ETickType TickType) { m_TickType = TickType; }

	static void TickObjects(float /*ElapsedSeconds*/) {}
protected:
	ETickType m_TickType = ETickType::Always;
private:
};

