#include "Engine/RHI/RHIBackend.h"
#include "Engine/Paths.h"

std::shared_ptr<RHIBackendConfiguration> RHIBackend::s_Configs;

RHIBackend::RHIBackend()
{
	s_Configs = RHIBackendConfiguration::Load(Paths::ConfigPath() / "RHIBackendConfig.json");
}
