#pragma once

#include "Core/Module.h"

class RenderService : public IService<RenderService>
{
public:
	void Initialize() override final;
	void Finalize() override final;

	inline class RHIBackend& GetBackend() { return *m_Backend; }
private:
	class RHIBackend* m_Backend = nullptr;
};
