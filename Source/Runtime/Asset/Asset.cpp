#include "Asset/Asset.h"
#include "Services/AssetDatabase.h"

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

bool AssetLoadRequest::Cancel()
{
	return AssetDatabase::Get().CancelLoad(*this);
}

bool AssetLoadRequest::Unload()
{
	return AssetDatabase::Get().Unload(*this);
}

void AssetLoadRequest::SetAssetStatus(Asset::EStatus Status)
{
	Target->SetStatus(Status);

	switch (Status)
	{
	case Asset::EStatus::Loading:
		InvokeAssetLoadCallback(OnLoading);
		break;
	case Asset::EStatus::Ready:
		InvokeAssetLoadCallback(OnLoaded);
		break;
	case Asset::EStatus::Canceled:
		InvokeAssetLoadCallback(OnCanceled);
		break;
	case Asset::EStatus::LoadFailed:
		InvokeAssetLoadCallback(OnLoadFailed);
		break;
	case Asset::EStatus::Unload:
		InvokeAssetLoadCallback(OnUnload);
		break;
	}
}