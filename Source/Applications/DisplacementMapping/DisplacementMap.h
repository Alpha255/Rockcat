#pragma once

#include "D3DApplication.h"

class AppDisplacement : public D3DApplication
{
public:
	AppDisplacement() = default;
	~AppDisplacement() = default;

	virtual void SetupScene();
	virtual void RenderScene();
protected:
private:
	bool m_bWireframe = false;
};
