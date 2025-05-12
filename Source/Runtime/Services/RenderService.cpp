#include "Services/RenderService.h"
#include "Application/BaseApplication.h"
#include "Application/ApplicationConfiguration.h"
#include "Services/SpdLogService.h"

void RenderService::Initialize()
{
	extern ApplicationRunner GApplicationRunner;
	auto& GApplication = GApplicationRunner.GetApplication();

}

void RenderService::Finalize()
{
	SafeDelete(m_Backend);
}
