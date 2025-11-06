#include "Asset/Asset.h"
#include "Services/SpdLogService.h"

std::shared_ptr<DataBlock> Asset::LoadData(AssetType::EContentsFormat ContentsFormat) const
{
	if (!std::filesystem::exists(GetPath()))
	{
		return nullptr;
	}

	auto Block = std::make_shared<DataBlock>(std::filesystem::file_size(GetPath()));
	std::ifstream FileStream(GetPath(), ContentsFormat == AssetType::EContentsFormat::Binary ? std::ios::in | std::ios::binary : std::ios::in);
	FileStream.read(reinterpret_cast<char*>(Block->Data.get()), Block->Size);
	FileStream.close();

	return Block;
}