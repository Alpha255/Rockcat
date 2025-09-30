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
			auto LastWriteTimeDuration = std::filesystem::last_write_time(Path).time_since_epoch();
			auto SysTimepoint = std::chrono::system_clock::time_point(std::chrono::duration_cast<std::chrono::system_clock::duration>(LastWriteTimeDuration));
			return std::chrono::system_clock::to_time_t(SysTimepoint);
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
	template<class T>
	inline void SetPath(T&& Path)
	{
		m_Path = std::forward<T>(Path);
	}

	static void SetLastWriteTime(const std::filesystem::path& Path, const std::chrono::system_clock::time_point& Timepoint = std::chrono::system_clock::from_time_t(0))
	{
		if (std::filesystem::exists(Path))
		{
			if (Timepoint.time_since_epoch().count() == 0)
			{
				std::filesystem::last_write_time(Path, std::filesystem::file_time_type::clock::now());
			}
			else
			{
				auto LastWriteTimeDuration = std::chrono::duration_cast<std::filesystem::file_time_type::duration>(Timepoint.time_since_epoch());
				auto FileTimepoint = std::filesystem::file_time_type::time_point(LastWriteTimeDuration);
				std::filesystem::last_write_time(Path, FileTimepoint);
			}
		}
	}

	void SetLastWriteTime(const std::chrono::system_clock::time_point& Timepoint = std::chrono::system_clock::from_time_t(0))
	{
		if (Timepoint.time_since_epoch().count() == 0)
		{
			auto Duration = std::filesystem::file_time_type::clock::now().time_since_epoch();
			auto SysTimepoint = std::chrono::system_clock::time_point(std::chrono::duration_cast<std::chrono::system_clock::duration>(Duration));
			m_LastWriteTime = std::chrono::system_clock::to_time_t(SysTimepoint);
		}
		else
		{
			m_LastWriteTime = std::chrono::system_clock::to_time_t(Timepoint);
		}
	}
private:
	std::filesystem::path m_Path; /// Notice the order of the members

	std::string m_Name;
	std::string m_Stem;
	std::string m_Extension;

	mutable std::time_t m_LastWriteTime = 0u;
};