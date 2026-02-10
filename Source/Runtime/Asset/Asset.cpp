#include "Asset/Asset.h"
#include "Services/SpdLogService.h"

std::shared_ptr<DataBlock> Asset::LoadData(bool IsBinary) const
{
	if (!std::filesystem::exists(GetPath()))
	{
		return nullptr;
	}

	auto Block = std::make_shared<DataBlock>(std::filesystem::file_size(GetPath()));
	std::ifstream FileStream(GetPath(), IsBinary ? std::ios::in | std::ios::binary : std::ios::in);
	FileStream.read(reinterpret_cast<char*>(Block->Data.get()), Block->Size);
	FileStream.close();

	return Block;
}

void Asset::OnStatusChanged(EStatus Status)
{
	switch (Status)
	{
	case EStatus::None:
		break;
	case EStatus::Loading:
		OnPreLoad();
		break;
	case EStatus::Ready:
		OnPostLoad();
		break;
	case EStatus::Canceled:
		OnCanceled();
		break;
	case EStatus::LoadFailed:
		OnLoadFailed();
		break;
	case EStatus::Unload:
		OnUnload();
		break;
	default:
		break;
	}
}