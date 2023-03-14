#pragma once

#include "Runtime/Core/File.h"

NAMESPACE_START(Gear)

class SyncContinuousOFStream : public File, public std::ofstream
{
public:
	SyncContinuousOFStream(const std::string& Path, File::ESerializeMode Mode = File::ESerializeMode::Text)
		: File(Path)
		, std::ofstream(Path, std::ios::out | (Mode == File::ESerializeMode::Binary ? std::ios::binary : 0))
	{
		assert(std::ofstream::is_open());
	}

	template<class T>
	void Write(const T& Value)
	{
		assert(std::is_trivial<T>());
		std::ofstream::write(reinterpret_cast<const char8_t*>(&Value), sizeof(T));
	}

	void Write(const void* Bytes, size_t Size)
	{
		std::ofstream::write(reinterpret_cast<const char8_t*>(Bytes), Size);
	}

	void AlignTo(size_t Size)
	{
		(void)Size;
	}

	virtual ~SyncContinuousOFStream()
	{
		std::ofstream::close();
	}
protected:
private:
};

class SyncContinuousIFStream : public File, public std::ifstream
{
public:
	SyncContinuousIFStream(const std::string& Path, File::ESerializeMode Mode = File::ESerializeMode::Text)
		: File(Path)
		, std::ifstream(Path, std::ios::in | (Mode == File::ESerializeMode::Binary ? std::ios::binary : 0))
	{
		assert(std::ifstream::is_open());
	}

	template<class T>
	T Read()
	{
		assert(std::is_trivial<T>());
		VERIFY((m_ReadSize + sizeof(T)) <= File::Size());

		T Data{};
		std::ifstream::read(reinterpret_cast<char8_t*>(&Data), sizeof(T));
		m_ReadSize += sizeof(T);

		return Data;
	}

	template<class T, size_t Size>
	std::array<T, Size> Read()
	{
		assert(std::is_trivial<T>());
		VERIFY((m_ReadSize + sizeof(T) * Size) <= File::Size());

		std::array<T, Size> Data{};
		std::ifstream::read(reinterpret_cast<char8_t*>(Data.data()), sizeof(T) * Size);
		m_ReadSize += (sizeof(T) * Size);

		return Data;
	}

	std::shared_ptr<byte8_t> ReadBytes(size_t Size)
	{
		VERIFY((m_ReadSize + Size) <= File::Size());

		std::shared_ptr<byte8_t> Bytes(new byte8_t[Size]());
		std::ifstream::read(reinterpret_cast<char8_t*>(Bytes.get()), Size);
		m_ReadSize += Size;

		return Bytes;
	}

	virtual ~SyncContinuousIFStream()
	{
		std::ifstream::close();
	}
protected:
private:
	size_t m_ReadSize = 0u;
};

NAMESPACE_END(Gear)