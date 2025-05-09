#pragma once

#include "Core/Tickable.h"
#include "World/WorldSettings.h"

class World final : public NoneCopyable, public ITickable
{
public:
	World();
	~World();

	void Tick(float ElapsedSeconds) override final;

	const WorldSettings& GetWorldSettings() const { assert(m_Settings); return *m_Settings; }
private:
	std::shared_ptr<WorldSettings> m_Settings;
};