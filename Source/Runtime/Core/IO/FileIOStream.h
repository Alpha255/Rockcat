#pragma once

#include "Runtime/Core/Definitions.h"

NAMESPACE_START(Gear)

struct IO
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

class IFileIOStream : public IO
{
public:
	IFileIOStream(const char8_t* FilePath)
		: m_FilePath(FilePath)
	{
	}

	IFileIOStream(const char8_t* FilePath, EOpenMode Mode = EOpenMode::Invalid)
		: m_FilePath(FilePath)
		, m_OpenMode(Mode)
	{
	}

	virtual ~IFileIOStream() { Close(); }

	virtual bool8_t Open(EOpenMode Mode) = 0;
	virtual void Close() = 0;
	virtual void Seek(size_t Bytes, ESeekMode Mode) = 0;
	virtual size_t Read(size_t Bytes, byte8_t* Buffer) = 0;
	virtual void Write(size_t Bytes, const byte8_t* Buffer) = 0;
	virtual bool8_t IsOpen() const = 0;

	bool8_t CanRead() const { return IsOpen() && EnumHasAnyFlags(m_OpenMode, EOpenMode::Read); }
	bool8_t CanWrite() const { return IsOpen() && EnumHasAnyFlags(m_OpenMode, EOpenMode::Write); }

	const char8_t* GetFilePath() const { return m_FilePath.c_str(); }
protected:
	EOpenMode m_OpenMode = EOpenMode::Invalid;
	std::string m_FilePath;
};

class StdFileIOStream : public IFileIOStream
{
public:
	using IFileIOStream::IFileIOStream;

	StdFileIOStream(const char8_t* FilePath, EOpenMode Mode = EOpenMode::Invalid)
		: IFileIOStream(FilePath, Mode)
	{
		Open(Mode);
	}

	bool8_t Open(EOpenMode Mode) override
	{
		if (IsOpen() && Mode == m_OpenMode) 
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

	size_t Read(size_t Bytes, byte8_t* Buffer) override
	{
		if (CanRead() && !m_Stream->eof())
		{
			return m_Stream->readsome(Buffer, static_cast<std::streamsize>(Bytes));
		}

		return 0u;
	}

	void Write(size_t Bytes, const byte8_t* Buffer) override
	{
		if (CanWrite())
		{
			m_Stream->write(Buffer, static_cast<std::streamsize>(Bytes));
		}
	}

	bool8_t IsOpen() const override
	{
		return m_Stream && m_Stream->is_open();
	}
protected:
private:
	std::shared_ptr<std::fstream> m_Stream;
};

NAMESPACE_END(Gear)