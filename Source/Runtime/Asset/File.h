#pragma once

#include "Core/Definitions.h"
#include "Core/Cereal.h"

class File
{
public:
	File(const std::filesystem::path& Path)
		: m_Path(Path)
		, m_Name(std::move(Path.filename().string()))
		, m_Stem(std::move(Path.stem().string()))
		, m_Extension(std::move(Path.extension().string()))
		, m_LastWriteTime(GetLastWriteTime(m_Path))
	{
	}

	File(std::filesystem::path&& Path)
		: m_Path(std::move(Path))
		, m_Name(std::move(m_Path.filename().string()))
		, m_Stem(std::move(m_Path.stem().string()))
		, m_Extension(std::move(m_Path.extension().string()))
		, m_LastWriteTime(GetLastWriteTime(m_Path))
	{
	}

	virtual ~File() = default;

	inline const std::filesystem::path& GetPath() const { return m_Path; }
	inline const std::string& GetName() const { return m_Name; }
	inline const std::string& GetStem() const { return m_Stem; }
	inline const std::string& GetExtension() const { return m_Extension; }
	inline std::time_t GetLastWriteTime() const { return m_LastWriteTime; }

	virtual bool IsDirty() const
	{
		std::time_t LastWriteTime = m_LastWriteTime;
		m_LastWriteTime = GetLastWriteTime(m_Path);
		return m_LastWriteTime != LastWriteTime;
	}

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_NVP(m_LastWriteTime)
		);
	}

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
private:
	std::filesystem::path m_Path; /// Notice the order of the members

	std::string m_Name;
	std::string m_Stem;
	std::string m_Extension;

	mutable std::time_t m_LastWriteTime = 0u;
};