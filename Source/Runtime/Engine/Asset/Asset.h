#pragma once

#include "Runtime/Core/ObjectID.h"

DECLARE_OBJECT_ID(Asset, uint32_t)

struct AssetData
{
	size_t SizeInBytes = 0u;
	std::shared_ptr<byte8_t> RawData;
};

struct AssetType
{
};

class Asset
{
public:
	enum class EAssetStatus : uint8_t
	{
		NotLoaded,
		Queued,
		Loading,
		Ready,
		Error
	};

	using AssetReadyCallback = std::function<void()>;
	using AssetSavedCallback = std::function<void()>;
	using AssetReloadedCallback = std::function<void()>;
	using AssetUnloadedCallback = std::function<void()>;
	using AssetErrorCallback = std::function<void()>;
	using AssetCanceledCallback = std::function<void()>;

	EAssetStatus GetStatus() const { return m_Status.load(); }
	bool8_t IsReady() const { return GetStatus() == EAssetStatus::Ready; }
	bool8_t IsLoading() const 
	{ 
		EAssetStatus Status = GetStatus();
		return Status == EAssetStatus::Loading || Status == EAssetStatus::Queued;
	}
	const AssetData* GetData() const { return &m_Data; }

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_NVP(m_LastWriteTime),
			CEREAL_NVP(m_Path)
		)
	}
protected:
	void SetStatus(EAssetStatus Status) { m_Status.store(Status); }
private:
	AssetData m_Data;
	std::atomic<EAssetStatus> m_Status = EAssetStatus::NotLoaded;

	uint64_t m_LastWriteTime = 0u;
	std::string m_Path;
};

