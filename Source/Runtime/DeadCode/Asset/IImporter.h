#pragma once

#include "Runtime/Asset/Asset.h"

namespace RHI
{
	class IDevice;
}

class IImporter
{
public:
	IImporter(RHI::IDevice* RHIDevice)
		: m_Device(RHIDevice)
	{
	}

	virtual void Reimport(std::shared_ptr<IAsset> Asset) = 0;
protected:
	RHI::IDevice* m_Device = nullptr;
};