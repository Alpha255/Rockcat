#pragma once

#include "Engine/RHI/RHIBuffer.h"

class RHIUploadManager
{
public:
	RHIUploadManager(class RHIDevice& Device)
		: m_Device(Device)
	{
	}
private:
	class RHIDevice& m_Device;
};