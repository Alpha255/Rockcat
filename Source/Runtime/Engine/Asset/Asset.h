#pragma once

#include "Runtime/Core/ObjectID.h"

DECLARE_OBJECT_ID(Asset, uint32_t)

#define ASSET_PATH_SHADERS      "Shaders\\"
#define ASSET_PATH_SHADERCACHE  "Shaders\\ShaderCache\\"
#define ASSET_PATH_TEXTURES     "Textures\\"
#define ASSET_PATH_IMAGES       "Assets\\Images\\"
#define ASSET_PATH_MATERIALS    "Materials\\"
#define ASSET_PATH_MODELS       "Models\\"
#define ASSET_PATH_AUDIOS       "Audios\\"
#define ASSET_PATH_SCENES       "Scenes\\"
#define ASSET_PATH_SETTINGS     "Settings\\"

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

	Asset(const char8_t* Path)
		: m_Path(Path)
		, m_LastWriteTime(GetFileLastWriteTime(Path))
	{
	}

	Asset(const std::string& Path)
		: m_Path(Path)
		, m_LastWriteTime(GetFileLastWriteTime(Path.c_str()))
	{
	}

	EAssetStatus GetStatus() const { return m_Status.load(); }
	bool8_t IsReady() const { return GetStatus() == EAssetStatus::Ready; }
	bool8_t IsLoading() const 
	{ 
		EAssetStatus Status = GetStatus();
		return Status == EAssetStatus::Loading || Status == EAssetStatus::Queued;
	}
	const AssetData* GetData() const { return &m_Data; }
	const char8_t* GetPath() const { return m_Path.c_str(); }
	std::time_t GetLastWriteTime() const { return m_LastWriteTime; }

	bool8_t IsDirty() const
	{
		if (!m_Dirty)
		{
			std::time_t LastWriteTime = m_LastWriteTime;
			m_LastWriteTime = GetFileLastWriteTime(m_Path.c_str());
			m_Dirty = m_LastWriteTime != LastWriteTime;
		}
		return m_Dirty;
	}

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_NVP(m_LastWriteTime)
		);
	}
protected:
	void SetStatus(EAssetStatus Status) { m_Status.store(Status); }

	static std::time_t GetFileLastWriteTime(const char8_t* Path)
	{
		if (std::filesystem::exists(Path))
		{
			return std::chrono::duration_cast<std::filesystem::file_time_type::duration>(
				std::filesystem::last_write_time(Path).time_since_epoch()).count();
		}
		return 0;
	}

	static size_t GetFileSize(const char8_t* Path)
	{
		if (std::filesystem::exists(Path))
		{
			return std::filesystem::file_size(Path);
		}
		return 0u;
	}

	static bool8_t IsFileExists(const char8_t* Path)
	{
		return std::filesystem::exists(Path);
	}
private:
	AssetData m_Data;
	std::atomic<EAssetStatus> m_Status = EAssetStatus::NotLoaded;

	mutable std::time_t m_LastWriteTime = 0u;
	std::string m_Path;
	mutable bool8_t m_Dirty = false;
};

