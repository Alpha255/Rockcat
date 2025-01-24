#include "Engine/Asset/Asset.h"
#include "Core/IO/FileIOStream.h"

std::optional<Asset::Callbacks> Asset::s_DefaultNullCallbacks(std::nullopt);

const DataBlock& Asset::GetRawData(AssetType::EContentsType ContentsType) const
{
	if (!m_RawData || !m_RawData->IsValid())
	{
		m_RawData = std::make_unique<DataBlock>(std::filesystem::file_size(m_Path));

		StdFileIOStream FileStream(m_Path, ContentsType == AssetType::EContentsType::Binary ?
			IO::EOpenMode::Read |
			IO::EOpenMode::Binary : IO::EOpenMode::Read);
		VERIFY(FileStream.Read(m_RawData->Size, reinterpret_cast<char*>(m_RawData->Data.get())) <= m_RawData->Size);
	}

	return *m_RawData;
}
