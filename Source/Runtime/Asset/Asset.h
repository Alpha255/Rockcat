#pragma once

#include "Asset/File.h"

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

	inline void Reset()
	{
		Size = Offset = 0u;
		Data.reset();
	}

	inline size_t GetSize() const { return Size; }
	inline size_t GetOffset() const { return Offset; }
	inline std::byte* GetRawData() const { return Data.get(); }

	template<class Archive>
	void load(Archive& Ar)
	{
		Ar(
			CEREAL_NVP(Size),
			CEREAL_NVP(Offset)
		);

		Data.reset(new std::byte[Size]());

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

	inline bool IsReady(std::memory_order Order = std::memory_order_relaxed) const { return GetStatus(Order) == EStatus::Ready; }
	inline bool IsLoading() const { return GetStatus() == EStatus::Loading; }

	std::shared_ptr<DataBlock> LoadData(bool IsBinary = true) const;

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

	inline EStatus GetStatus(std::memory_order Order = std::memory_order_relaxed) const { return m_Status.load(Order); }
	inline void SetStatus(EStatus Status) { m_Status.store(Status, std::memory_order_release); }

	std::atomic<EStatus> m_Status{ EStatus::None };
};

struct AssetLoadRequest
{
	using AssetLoadCallback = std::function<void(Asset&)>;

	std::string_view Path;
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

class AssetLoader
{
public:
	AssetLoader(std::vector<std::string_view>&& SupportedFormats)
		: m_SupportedFormats(std::move(SupportedFormats))
	{
	}

	inline bool IsSupportedFormat(std::string_view Extension) const
	{
		return std::find_if(m_SupportedFormats.begin(), m_SupportedFormats.end(), [Extension](const std::string_view& Ext) {
				return _stricmp(Ext.data(), Extension.data()) == 0;
			}) != m_SupportedFormats.end();
	}

	virtual bool Load(Asset& Target) = 0;
protected:
	friend class AssetDatabase;

	virtual std::shared_ptr<Asset> CreateAsset(const std::filesystem::path& Path) = 0;
private:
	std::vector<std::string_view> m_SupportedFormats;
};
