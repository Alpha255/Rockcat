#pragma once

#include "Core/ObjectID.h"

struct DataBlock
{
	size_t Size = 0u;
	size_t Offset = 0u;
	std::shared_ptr<std::byte> Data;

	DataBlock() = default;

	DataBlock(size_t InSize, const void* InData = nullptr, size_t InOffset = 0u)
		: Size(InSize)
		, Offset(InOffset)
		, Data(new std::byte[InSize + InOffset]())
	{
		assert(Offset < Size);

		if (InData)
		{
			VERIFY(memcpy_s(Data.get() + Offset, InSize, InData, InSize) == 0);
		}
	}
	
	inline bool IsValid() const { return Size && Data; }

	template<class Archive>
	void load(Archive& Ar)
	{
		Ar(
			CEREAL_NVP(Size),
			CEREAL_NVP(Offset)
		);

		Data.reset(new std::byte[Size + Offset]());

		Ar.loadBinaryValue(Data.get() + Offset, Size, "Data");
	}

	template<class Archive>
	void save(Archive& Ar) const
	{
		Ar(
			CEREAL_NVP(Size),
			CEREAL_NVP(Offset)
		);

		Ar.saveBinaryValue(Data.get() + Offset, Size, "Data");
	}
};

struct AssetType
{
	enum class EContentsType
	{
		Text,
		Binary
	};

	std::string_view Name;
	std::filesystem::path Extension;
	EContentsType ContentsType = EContentsType::Binary;
};

class Asset
{
public:
	enum class EStatus : uint8_t
	{
		NotLoaded,
		Loading,
		Ready,
		LoadFailed
	};

	using AssetLoadCallback = std::function<void(Asset&)>;

	struct AssetLoadCallbacks
	{
		AssetLoadCallback PreLoadCallback;
		AssetLoadCallback ReadyCallback;
		AssetLoadCallback ReloadCallback;
		AssetLoadCallback LoadFailedCallback;
		AssetLoadCallback CanceledCallback;
		AssetLoadCallback UnloadedCallback;
	};

	Asset(const std::filesystem::path& Path)
		: m_Path(Path)
		, m_LastWriteTime(GetLastWriteTime(m_Path))
	{
	}

	Asset(std::filesystem::path&& Path)
		: m_Path(std::move(Path))
		, m_LastWriteTime(GetLastWriteTime(m_Path))
	{
	}

	virtual ~Asset() = default;

	EStatus GetStatus() const { return m_Status.load(std::memory_order_relaxed); }
	bool IsReady() const { return GetStatus() == EStatus::Ready; }

	const std::filesystem::path& GetPath() const { return m_Path; }
	std::filesystem::path GetName() const { return m_Path.filename(); }
	std::filesystem::path GetStem() const { return m_Path.stem(); }
	std::filesystem::path GetExtension() const { return m_Path.extension(); }
	std::time_t GetLastWriteTime() const { return m_LastWriteTime; }

	std::shared_ptr<DataBlock> LoadData(AssetType::EContentsType ContentsType) const;

	virtual bool IsDirty() const
	{
		std::time_t LastWriteTime = m_LastWriteTime;
		m_LastWriteTime = GetLastWriteTime(m_Path);
		return m_LastWriteTime != LastWriteTime;
	}

	void SetLoadCallbacks(std::optional<AssetLoadCallbacks>& Callbacks)
	{ 
		if (Callbacks)
		{ 
			m_LoadCallbacks = std::move(Callbacks.value());
		} 
	}

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_NVP(m_LastWriteTime)
		);
	}

	static std::optional<AssetLoadCallbacks> s_DefaultLoadCallbacks;

	static std::time_t GetLastWriteTime(const std::filesystem::path& Path)
	{
		if (std::filesystem::exists(Path))
		{
			return std::chrono::duration_cast<std::filesystem::file_time_type::duration>(
				std::filesystem::last_write_time(Path).time_since_epoch()).count();
		}
		return 0;
	}

	static size_t GetSize(const std::filesystem::path& Path)
	{
		if (std::filesystem::exists(Path))
		{
			return std::filesystem::file_size(Path);
		}
		return 0u;
	}
protected:
	friend class AssetImportTask;

	void SetStatus(EStatus Status) { m_Status.store(Status, std::memory_order_release); }

	virtual void OnPreLoad()
	{
		SetStatus(EStatus::Loading);

		if (m_LoadCallbacks.PreLoadCallback)
		{
			m_LoadCallbacks.PreLoadCallback(*this);
		}
	}
	virtual void OnReady()
	{
		SetStatus(EStatus::Ready);

		if (m_LoadCallbacks.ReadyCallback)
		{
			m_LoadCallbacks.ReadyCallback(*this);
		}
	}
	virtual void OnLoadFailed()
	{
		SetStatus(EStatus::LoadFailed);

		if (m_LoadCallbacks.LoadFailedCallback)
		{
			m_LoadCallbacks.LoadFailedCallback(*this);
		}
	}
	virtual void OnLoadCanceled()
	{
		SetStatus(EStatus::NotLoaded);

		if (m_LoadCallbacks.CanceledCallback)
		{
			m_LoadCallbacks.CanceledCallback(*this);
		}
	}
	virtual void OnUnloaded()
	{
		SetStatus(EStatus::NotLoaded);

		if (m_LoadCallbacks.UnloadedCallback)
		{
			m_LoadCallbacks.UnloadedCallback(*this);
		}
	}

	std::atomic<EStatus> m_Status = EStatus::NotLoaded;

	AssetLoadCallbacks m_LoadCallbacks;

	std::filesystem::path m_Path; /// Notice the order of the members
	mutable std::time_t m_LastWriteTime = 0u;
};

class IAssetImporter
{
public:
	IAssetImporter(std::vector<AssetType>&& ValidAssetTypes)
		: m_ValidAssetTypes(std::move(ValidAssetTypes))
	{
	}

	const AssetType* FindValidAssetType(const std::filesystem::path& Path) const
	{
		auto It = std::find_if(m_ValidAssetTypes.begin(), m_ValidAssetTypes.end(), [&Path](const AssetType& Type) {
			return Path.extension() == Type.Extension;
		});
		return It == m_ValidAssetTypes.cend() ? nullptr : &(*It);
	}

	virtual std::shared_ptr<Asset> CreateAsset(const std::filesystem::path& Path) = 0;
	virtual bool Reimport(Asset& InAsset, const AssetType& InType) = 0;
protected:
private:
	std::vector<AssetType> m_ValidAssetTypes;
};

template<class T>
struct LoadAssetRequest
{
	Asset::AssetLoadCallback PreLoadCallback;
	Asset::AssetLoadCallback ReadyCallback;
	Asset::AssetLoadCallback ReloadCallback;
	Asset::AssetLoadCallback LoadFailedCallback;
	Asset::AssetLoadCallback CanceledCallback;
	Asset::AssetLoadCallback UnloadedCallback;

	std::shared_ptr<T> Asset;

	std::string Message;

	inline bool IsDone() const { return Asset && Asset->IsReady(); }
};
