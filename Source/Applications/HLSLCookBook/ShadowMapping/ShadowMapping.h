#pragma once

#include "Public/IRenderApplication.h"

class ShadowMapping : public IRenderApplication
{
public:
	ShadowMapping() = default;
	~ShadowMapping() = default;

	void PrepareScene() override;
	void RenderScene() override;
protected:
private:
};
