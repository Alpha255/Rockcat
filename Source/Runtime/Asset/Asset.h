#pragma once

#include "Asset/File.h"
#include "Core/Spdlogging.h"

class Asset : public File
{
public:
	enum class EStatus : uint8_t
	{
		None,
		Loading,
		Ready,
		Canceled,
		LoadFailed,
		Unload
	};

	using File::File;

	inline bool IsReady() const { return GetStatus() == EStatus::Ready; }
	inline bool IsLoading() const { return GetStatus() == EStatus::Loading; }

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_BASE(File)
		);
	}
protected:
	friend class AssetLoader;
	friend struct AssetLoadRequest;

	inline EStatus GetStatus() const { return m_Status.load(std::memory_order_acquire); }
	inline void SetStatus(EStatus Status) { m_Status.store(Status, std::memory_order_release); }

	std::atomic<EStatus> m_Status{ EStatus::None };
};

struct AssetLoadRequest
{
	using AssetLoadCallback = std::function<void(Asset&)>;

	string Path;
	bool ForceReload = false;
	bool Async = true;
	std::shared_ptr<Asset> Target;

	AssetLoadCallback OnLoading;
	AssetLoadCallback OnLoaded;
	AssetLoadCallback OnLoadFailed;
	AssetLoadCallback OnCanceled;
	AssetLoadCallback OnUnload;

	bool Cancel();
	bool Unload();
protected:
	friend class AssetDatabase;
	
	inline void InvokeAssetLoadCallback(AssetLoadCallback& Func)
	{
		if (Func)
		{
			Func(*Target);
		}
	}

	void SetAssetStatus(Asset::EStatus Status);
};
using AssetLoadRequests = std::vector<AssetLoadRequest>;


DECLARE_LOGGER_CATEGORY(LogAsset);