#include "Engine/Services/RenderService.h"
#include "Engine/Services/SpdLogService.h"
#include "Engine/Engine.h"
#include "RHI/Vulkan/VulkanRHI.h"

RHIBackend* RenderService::GetOrCreateBackend(ERHIBackend Backend)
{
	assert(Backend < ERHIBackend::Num);

#if !ENABLE_MULTI_RENDERER
	m_Backend = Backend;
#endif

	if (!m_Backends[Backend])
	{
		switch (Backend)
		{
		case ERHIBackend::Software:
			break;
		case ERHIBackend::Vulkan:
			m_Backends[ERHIBackend::Vulkan] = std::make_unique<VulkanRHI>();
			break;
		case ERHIBackend::D3D11:
			break;
		case ERHIBackend::D3D12:
			break;
		}

		if (!m_Backends[Backend])
		{
			LOG_CRITICAL("Render backend \"{}\" is not support yet!", RHIBackend::GetName(Backend));
		}
		else
		{
			m_Backends[Backend]->InitializeGraphicsDevice();
			m_Backends[Backend]->PrepareGlobalResources();
		}
	}

	return m_Backends[Backend].get();
}

void RenderService::OnStartup()
{
	for (auto& Application : Engine::Get().GetApplications())
	{
		Application->SetRenderBackend(GetOrCreateBackend(Application->GetRenderSettings().Backend));
	}
}

void RenderService::OnShutdown()
{
	for (auto& Backend : m_Backends)
	{
		Backend.reset();
	}
}
