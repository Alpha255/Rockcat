#pragma once

#include "D3DApplication.h"

#include "ParticleSystem.h"

class AppParticleSystem : public D3DApplication
{
public:
	AppParticleSystem() = default;
	~AppParticleSystem() = default;

	virtual void SetupScene();
	virtual void RenderScene();
	virtual void UpdateScene(float elapseTime, float totalTime);
protected:
private:
	ParticleSystem m_ParticleSystem;
};
