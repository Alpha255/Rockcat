#include "Services/RenderService.h"
#include "Application/BaseApplication.h"
#include "Application/ApplicationConfiguration.h"
#include "Services/ShaderLibrary.h"
#include "Services/SpdLogService.h"
#include "RHI/RHIUploadManager.h"
#include "RHI/Vulkan/VulkanRHI.h"

void RenderService::OnStartup()
{
	extern ApplicationRunner GApplicationRunner;
	auto& GApplication = GApplicationRunner.GetApplication();

	switch (GApplication.GetRenderSettings().Backend)
	{
	case ERHIBackend::Software:
		break;
	case ERHIBackend::Vulkan:
		m_Backend = new VulkanRHI();
		break;
	case ERHIBackend::D3D11:
		break;
	case ERHIBackend::D3D12:
		break;
	default:
		break;
	}

	if (!m_Backend)
	{
		LOG_CRITICAL("Render backend \"{}\" is not support yet!", RHIBackend::GetName(GApplication.GetRenderSettings().Backend));
	}
	else
	{
		m_Backend->Initialize(GApplication.GetWindow(), GApplication.GetRenderSettings());
		ShaderLibrary::Create(m_Backend->GetType(), m_Backend->GetDevice());
		RHIUploadManager::Create(m_Backend->GetDevice());
		
		m_Backend->PrepareGlobalResources();
	}
}

void RenderService::OnShutdown()
{
	SafeDelete(m_Backend);
}
