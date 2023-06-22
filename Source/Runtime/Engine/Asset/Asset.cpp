#include "Runtime/Engine/Asset/Asset.h"
#include "Runtime/Core/IO/FileIOStream.h"

void Asset::ReadRawData()
{
	m_RawData.Allocate(std::filesystem::file_size(m_Path));

	StdFileIOStream FileStream(m_Path, IO::EOpenMode::Read);
	VERIFY(FileStream.Read(m_RawData.SizeInBytes, m_RawData.Data.get()) == m_RawData.SizeInBytes);
}
