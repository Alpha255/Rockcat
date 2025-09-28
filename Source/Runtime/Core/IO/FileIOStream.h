#pragma once

#include "Core/Definitions.h"

struct FileIO
{
	enum class EOpenMode : uint8_t
	{
		Invalid,
		Read = std::ios::in,
		Write = std::ios::out,
		SeekToEnd = std::ios::ate,
		Append = std::ios::app,
		Discard = std::ios::trunc,
		Binary = std::ios::binary
	};

	enum class ESeekMode : uint8_t
	{
		Begin = std::ios::beg,
		Current = std::ios::cur,
		End = std::ios::end
	};
};
ENUM_FLAG_OPERATORS(FileIO::EOpenMode)

class FileStream : public FileIO
{
public:
	template<class T>
	FileStream(T&& FilePath, EOpenMode Mode)
		: m_FilePath(std::forward<T>(FilePath))
		, m_OpenMode(Mode)
	{
	}

	virtual ~FileStream() = default;

	virtual bool Open(EOpenMode Mode) = 0;
	virtual void Close() = 0;
	virtual void Seek(size_t Bytes, ESeekMode Mode) = 0;
	virtual size_t Read(size_t Bytes, char* Buffer) = 0;
	virtual void Write(size_t Bytes, const char* Buffer) = 0;
	virtual bool IsOpen() const = 0;

	inline bool CanRead() const { return IsOpen() && EnumHasAnyFlags(m_OpenMode, EOpenMode::Read); }
	inline bool CanWrite() const { return IsOpen() && EnumHasAnyFlags(m_OpenMode, EOpenMode::Write); }

	inline const std::filesystem::path& GetFilePath() const { return m_FilePath; }
protected:
	EOpenMode m_OpenMode = EOpenMode::Invalid;
	std::filesystem::path m_FilePath;
};

class StdFileStream : public FileStream
{
public:
	template<class T>
	StdFileStream(T&& FilePath, EOpenMode Mode)
		: FileStream(std::forward<T>(FilePath), Mode)
	{
		Open(Mode);
	}

	~StdFileStream()
	{
		if (IsOpen())
		{
			Close();
		}
	}

	bool Open(EOpenMode Mode) override
	{
		if (IsOpen() && Mode == m_OpenMode) 
		{ 
			return false; 
		}

		if (Mode == EOpenMode::Invalid)
		{
			return false;
		}

		m_OpenMode = Mode;

		if (m_Stream)
		{
			m_Stream->close();
		}

		m_Stream = std::make_shared<std::fstream>(m_FilePath, static_cast<std::ios_base::openmode>(m_OpenMode));

		return m_Stream->good();
	}

	void Close() override
	{
		if (IsOpen())
		{
			m_Stream->close();
		}
	}

	void Seek(size_t Bytes, ESeekMode Mode) override
	{
		if (CanRead())
		{
			m_Stream->seekg(static_cast<std::streamoff>(Bytes), static_cast<std::ios_base::seekdir>(Mode));
		}
		else if (CanWrite())
		{
			m_Stream->seekp(static_cast<std::streamoff>(Bytes), static_cast<std::ios_base::seekdir>(Mode));
		}
	}

	size_t Read(size_t Bytes, char* Buffer) override
	{
		if (CanRead() && !m_Stream->eof())
		{
			return m_Stream->read(Buffer, static_cast<std::streamsize>(Bytes)).gcount();
		}

		return 0u;
	}

	void Write(size_t Bytes, const char* Buffer) override
	{
		if (CanWrite())
		{
			m_Stream->write(Buffer, static_cast<std::streamsize>(Bytes));
		}
	}

	bool IsOpen() const override
	{
		return m_Stream && m_Stream->is_open();
	}
protected:
private:
	std::shared_ptr<std::fstream> m_Stream;
};

