#pragma once

#include "Runtime/Engine/RHI/RHIInterface.h"

class VulkanRHI final : public RHIInterface
{
public:
	VulkanRHI();

	ERenderHardwareInterface GetRHIType() const override final { return ERenderHardwareInterface::Vulkan; }
protected:
	void InitializeGraphicsDevices() override final;

	void Finalize() override final;
private:
	std::unique_ptr<class VulkanDevice> m_Device;
};
