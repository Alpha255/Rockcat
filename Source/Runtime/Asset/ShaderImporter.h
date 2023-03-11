#pragma once

#include "Runtime/Asset/IImporter.h"

class ShaderImporter : public IImporter
{
public:
	ShaderImporter(RHI::IDevice* RHIDevice);

	void Reimport(std::shared_ptr<IAsset> Asset) override final;
};
