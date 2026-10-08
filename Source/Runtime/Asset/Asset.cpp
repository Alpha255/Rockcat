#include "Asset/Asset.h"
#include "Asset/AssetDatabase.h"

DEFINE_LOGGER_CATEGORY(LogAsset);

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
	assert(Target);

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