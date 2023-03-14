#pragma once

#include "Runtime/Core/String.h"

NAMESPACE_START(Gear)

struct FileTime
{
	uint16_t Year = 0u;
	uint16_t Month = 0u;
	uint16_t Day = 0u;
	uint16_t Hour = 0u;
	uint16_t Minutes = 0u;
	uint16_t Seconds = 0u;

	bool8_t operator==(const FileTime& Other)
	{
		return Year == Other.Year &&
			Month == Other.Month &&
			Day == Other.Day &&
			Hour == Other.Hour &&
			Minutes == Other.Minutes &&
			Seconds == Other.Seconds;
	}

	bool8_t operator!=(const FileTime& Other)
	{
		return !(*this == Other);
	}

	friend bool8_t operator==(const FileTime& Left, const FileTime& Right)
	{
		return Left.Year == Right.Year &&
			Left.Month == Right.Month &&
			Left.Day == Right.Day &&
			Left.Hour == Right.Hour &&
			Left.Minutes == Right.Minutes &&
			Left.Seconds == Right.Seconds;
	}

	friend bool8_t operator!=(const FileTime& Left, const FileTime& Right)
	{
		return !(Left == Right);
	}

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_NVP(Year),
			CEREAL_NVP(Month),
			CEREAL_NVP(Day),
			CEREAL_NVP(Hour),
			CEREAL_NVP(Minutes),
			CEREAL_NVP(Seconds)
		);
	}
};

class File
{
public:
	enum class ESerializeMode : uint8_t
	{
		Text,
		Binary
	};

	File(const File&) = default;
	virtual ~File() = default;

	explicit File(const char8_t* Path)
		: m_Path(Path ? Path : "")
	{
		String::Replace(m_Path, "/", "\\");

		LastWriteTime();
	}

	explicit File(const std::string& Path)
		: File(Path.c_str())
	{
	}

	File(File&& Other) noexcept
		: m_Size(Other.m_Size)
		, m_Path(std::move(Other.m_Path))
		, m_RelativePath(std::move(Other.m_RelativePath))
		, m_Name(std::move(Other.m_Name))
		, m_Extension(std::move(Other.m_Extension))
		, m_LastWriteTime(std::move(Other.m_LastWriteTime))
	{
		std::exchange(Other.m_Size, {});
	}

	File& operator=(const File& Other)
	{
		m_Size = Other.m_Size;
		m_Path = Other.m_Path;
		m_RelativePath = Other.m_RelativePath;
		m_Name = Other.m_Name;
		m_Extension = Other.m_Extension;
		m_LastWriteTime = Other.m_LastWriteTime;
		return *this;
	}

	File& operator=(File&& Other) noexcept
	{
		m_Size = std::exchange(Other.m_Size, 0u);
		m_Path = std::move(Other.m_Path);
		m_RelativePath = std::move(Other.m_RelativePath);
		m_Name = std::move(Other.m_Name);
		m_Extension = std::move(Other.m_Extension);
		m_LastWriteTime = std::move(Other.m_LastWriteTime);
		return *this;
	}

	static std::string Name(const std::string& Path, bool8_t ToLower = false)
	{
		auto Ret = std::filesystem::path(Path).filename().string();
		return ToLower ? String::Lowercase(Ret) : Ret;
	}

	static std::string NameWithoutExtension(const std::string& Path, bool8_t ToLower = false)
	{
		auto Ret = std::filesystem::path(Path).stem().string();
		return ToLower ? String::Lowercase(Ret) : Ret;
	}

	static std::string Extension(const std::string& Path, bool8_t ToLower = false)
	{
		auto Ret = std::filesystem::path(Path).extension().string();
		return ToLower ? String::Lowercase(Ret) : Ret;
	}

	static std::string Directory(const std::string& Path, bool8_t Relative = false, bool8_t ToLower = false)
	{
		auto Ret = std::filesystem::path(Path).parent_path();
		if (Relative)
		{
			Ret = Ret.relative_path();
		}
		return ToLower ? String::Lowercase(Ret.string()) : Ret.string();
	}

	static FileTime LastWriteTime(const char8_t* Path);

	static bool8_t Exists(const char8_t* Path);

	static size_t Size(const char8_t* Path);

	static std::vector<std::string> GetFileList(const char8_t* Path, const std::vector<std::string>& Filters, bool8_t ToLower = false);

	static std::string Find(const char8_t* Path, const char8_t* Name);

	static void CreateDirectories(const char8_t* Path);

	inline const char8_t* Name() const
	{
		if (m_Name.empty())
		{
			m_Name = NameWithoutExtension(m_Path);
		}

		return m_Name.c_str();
	}

	inline const char8_t* Extension() const
	{
		if (m_Extension.empty())
		{
			m_Extension = Extension(m_Path, true);
		}

		return m_Extension.c_str();
	}

	inline const char8_t* Path() const
	{
		return m_Path.c_str();
	}

	inline const char8_t* RelativePath() const
	{
		if (m_RelativePath.empty())
		{
			m_RelativePath = m_Path.substr(m_Path.find("\\") + 1u);
		}

		return m_RelativePath.c_str();
	}

	size_t Size()
	{
		assert(Exists(m_Path.c_str()));
		if (!m_Size)
		{
			m_Size = Size(m_Path.c_str());
		}

		return m_Size;
	}

	std::unique_ptr<byte8_t> Read(ESerializeMode SerializeMode = ESerializeMode::Text)
	{
		int32_t Mode = (SerializeMode == ESerializeMode::Binary ? (std::ios::in | std::ios::binary) : std::ios::in);

		std::ifstream Fs(m_Path, Mode);
		assert(Fs.is_open());

		std::unique_ptr<byte8_t> Data(new byte8_t[Size()]());
		Fs.read(reinterpret_cast<char8_t*>(Data.get()), Size());
		Fs.close();

		return Data;
	}

	std::tuple<bool8_t, FileTime> LastWriteTime() const
	{
		bool8_t Dirty = false;
		auto LastWriteTime = File::LastWriteTime(m_Path.c_str());
		if (m_LastWriteTime != LastWriteTime)
		{
			m_LastWriteTime = LastWriteTime;
			Dirty = true;
		}
		return std::make_tuple(Dirty, LastWriteTime);
	}

	bool8_t IsDirty() const
	{
		bool8_t Dirty = false;

		auto LastWriteTime = File::LastWriteTime(m_Path.c_str());
		if (m_LastWriteTime != LastWriteTime)
		{
			m_LastWriteTime = LastWriteTime;
			Dirty = true;
		}

		return Dirty;
	}

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_NVP(m_Path),
			CEREAL_NVP(m_LastWriteTime)
		);
	}
protected:
	friend class cereal::access;

	File() = default;
private:
	size_t m_Size = 0u;
	std::string m_Path;
	mutable std::string m_RelativePath;
	mutable std::string m_Name;
	mutable std::string m_Extension;
	mutable FileTime m_LastWriteTime;
};

NAMESPACE_END(Gear)
