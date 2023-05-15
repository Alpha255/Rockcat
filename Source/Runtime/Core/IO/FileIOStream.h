#pragma once

#include "Runtime/Core/Definitions.h"

NAMESPACE_START(Gear)

struct IO
{
	enum class EOpenMode : uint8_t
	{
		Invalid,
		Append = 1 << 0,
		Binary = 1 << 1,
		Read = 1 << 2,
		Write = 1 << 3,
		Discard = 1 << 4,
		OpenToEnd = 1 << 5
	};

	enum class ESeekMode : uint8_t
	{
		Begin,
		Current,
		End
	};
};

class IFileIOStream : public IO
{
public:
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
	virtual size_t Write(size_t Bytes, const byte8_t* Buffer) = 0;
	virtual bool8_t IsOpen() const = 0;

	bool8_t CanRead() const { return EnumHasAnyFlags(m_OpenMode, EOpenMode::Read); }
	bool8_t CanWrite() const { return EnumHasAnyFlags(m_OpenMode, EOpenMode::Write) || EnumHasAnyFlags(m_OpenMode, EOpenMode::Append); }

	const char8_t* GetFilePath() const { return m_FilePath.c_str(); }
protected:
private:
	std::string m_FilePath;
	EOpenMode m_OpenMode = EOpenMode::Invalid;
};

class StdFileIOStream : public IFileIOStream
{
public:
	using IFileIOStream::IFileIOStream;

	bool8_t Open(EOpenMode Mode) override
	{

	}

	void Close() override
	{

	}

	void Seek(size_t Bytes, ESeekMode Mode) override
	{

	}

	size_t Read(size_t Bytes, byte8_t* Buffer) override
	{

	}

	size_t Write(size_t Bytes, const byte8_t* Buffer) override
	{

	}

	bool8_t IsOpen() const override
	{

	}
protected:
private:
	std::shared_ptr<std::fstream> m_Stream;
};

NAMESPACE_END(Gear)