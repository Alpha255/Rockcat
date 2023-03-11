#pragma once

#include "Runtime/Asset/IImporter.h"

class ImageImporter : public IImporter
{
public:
	ImageImporter(RHI::IDevice* RHIDevice);

	void Reimport(std::shared_ptr<IAsset> Asset) override final;
protected:
private:
	std::vector<std::pair<std::vector<std::string_view>, std::shared_ptr<class ITextureImporter>>> m_Importers;
};
