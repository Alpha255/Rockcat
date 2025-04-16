#include "Engine/Services/RenderService.h"
#include "Engine/Application/BaseApplication.h"
#include "Engine/Application/ApplicationConfiguration.h"
#include "Engine/Services/ShaderLibrary.h"
#include "Engine/Services/SpdLogService.h"
#include "Engine/RHI/RHIUploadManager.h"
#include "RHI/Vulkan/VulkanRHI.h"

void RenderService::OnStartup()
{
	extern ApplicationRunner GApplicationRunner;

	switch (GApplicationRunner.GetApplication().GetRenderSettings().Backend)
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
		LOG_CRITICAL("Render backend \"{}\" is not support yet!", RHIBackend::GetName(GApplicationRunner.GetApplication().GetRenderSettings().Backend));
	}
	else
	{
		m_Backend->InitializeGraphicsDevice();
		ShaderLibrary::Create(m_Backend->GetType(), m_Backend->GetDevice());
		RHIUploadManager::Create(m_Backend->GetDevice());
		
		m_Backend->PrepareGlobalResources();
	}
}

void RenderService::OnShutdown()
{
	SafeDelete(m_Backend);
}
