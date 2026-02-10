#pragma once

#include "Asset/File.h"
#include "Core/StringUtils.h"

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

struct AssetType
{
	enum class EContentsFormat
	{
		PlainText,
		Binary
	};

	std::string_view Description;
	std::string_view Extension;
	EContentsFormat ContentsFormat = EContentsFormat::Binary;
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

	inline EStatus GetStatus(std::memory_order Order = std::memory_order_relaxed) const { return m_Status.load(Order); }

	inline void SetStatus(EStatus Status)
	{
		m_Status.store(Status, std::memory_order_release);
		OnStatusChanged(Status);
	}

	void OnStatusChanged(EStatus Status);

	virtual void OnPreLoad() {}
	virtual void OnCanceled() {}
	virtual void OnPostLoad() {}
	virtual void OnUnload() {}
	virtual void OnLoadFailed() {}

	std::atomic<EStatus> m_Status{ EStatus::None };
};

struct AssetLoadRequest
{
	std::string_view Path;
	bool ForceReload = false;
	bool Async = true;
};

class AssetLoader
{
public:
	AssetLoader(std::vector<std::string_view>&& SupportedFormats)
		: m_SupportedFormats(std::move(SupportedFormats))
	{
	}

	inline bool IsSupportedFormat(const std::filesystem::path& Path) const
	{
		return std::find(
			m_SupportedFormats.begin(), 
			m_SupportedFormats.end(), 
			String::Lowercase(Path.extension().string())) != m_SupportedFormats.end();
	}

	virtual bool Load(Asset& InAsset, const AssetType& InType) = 0;

	virtual bool TryLoad(AssetLoadRequest) { return false; }
protected:
	virtual std::shared_ptr<Asset> CreateAsset(const std::filesystem::path& Path) = 0;
private:
	std::vector<std::string_view> m_SupportedFormats;
};
