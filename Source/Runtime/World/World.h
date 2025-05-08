#pragma once

#include "Core/Tickable.h"

class World final : public NoneCopyable, public ITickable
{
public:
	World();
	~World();

	void Tick(float ElapsedSeconds) override final;
};