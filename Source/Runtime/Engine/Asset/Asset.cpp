#include "Engine/Asset/Asset.h"
#include "Core/IO/FileIOStream.h"

std::optional<Asset::Callbacks> Asset::s_DefaultNullCallbacks(std::nullopt);

void Asset::ReadRawData(AssetType::EContentsType ContentsType)
{
	if (m_RawData.SizeInBytes == 0u)
	{
		m_RawData.Allocate(std::filesystem::file_size(m_Path));

		StdFileIOStream FileStream(m_Path, ContentsType == AssetType::EContentsType::Binary ?
			IO::EOpenMode::Read |
			IO::EOpenMode::Binary : IO::EOpenMode::Read);
		VERIFY(FileStream.Read(m_RawData.SizeInBytes, m_RawData.Data.get()) <= m_RawData.SizeInBytes);
	}
}
