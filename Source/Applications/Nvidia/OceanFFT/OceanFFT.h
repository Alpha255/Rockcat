#pragma once

#include "D3DApplication.h"

class AppOceanFFT : public D3DApplication
{
public:
	AppOceanFFT() = default;
	~AppOceanFFT() = default;

	virtual void SetupScene();
	virtual void RenderScene();
	virtual void ResizeWindow(uint32_t width, uint32_t height);
protected:
private:
};
