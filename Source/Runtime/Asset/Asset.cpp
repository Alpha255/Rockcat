#include "Asset/Asset.h"
#include "Core/IO/FileIOStream.h"

std::optional<Asset::AssetLoadCallbacks> Asset::s_DefaultLoadCallbacks(std::nullopt);

std::shared_ptr<DataBlock> Asset::LoadData(AssetType::EContentsType ContentsType) const
{
	auto Block = std::make_shared<DataBlock>(std::filesystem::file_size(m_Path));

	StdFileIOStream FileStream(m_Path, ContentsType == AssetType::EContentsType::Binary ?
		IO::EOpenMode::Read |
		IO::EOpenMode::Binary : IO::EOpenMode::Read);
	VERIFY(FileStream.Read(Block->Size, reinterpret_cast<char*>(Block->Data.get())) <= Block->Size);

	return Block;
}
