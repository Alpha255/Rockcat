#pragma once

#include "RenderApp.h"

class AppAdaptiveTessellation : public RenderApp
{
public:
	AppAdaptiveTessellation() = default;
	~AppAdaptiveTessellation() = default;

	virtual void SetupScene();
	virtual void RenderScene();

	virtual void ResizeWindow(uint32_t width, uint32_t height);
protected:
private:
	bool m_bEnableTess = false;
	int32_t m_PartitioningMode = 0;
	uint32_t m_IndexCount = 0U;
	uint32_t m_VertexCount = 0U;
};
