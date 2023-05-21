#if 0
#include "Runtime/Core/File.h"
#include "Runtime/Core/Logger.h"
#include "Runtime/Core/Platform.h"

#if PLATFORM_WIN32

size_t File::Size(const char8_t* Path)
{
	if (Exists(Path))
	{
		return std::filesystem::file_size(Path);
	}

	return 0u;
}

bool8_t File::Exists(const char8_t* Path)
{
	return std::filesystem::exists(Path);
}

FileTime File::LastWriteTime(const char8_t* Path)
{
	if (Exists(Path))
	{
		auto LastWriteTime = std::filesystem::last_write_time(Path);
		::SYSTEMTIME SysTime;
		VERIFY_WITH_PLATFORM_MESSAGE(::FileTimeToSystemTime(reinterpret_cast<const ::FILETIME*>(&LastWriteTime), &SysTime) != 0);

		return FileTime
		{
			SysTime.wYear,
			SysTime.wMonth,
			SysTime.wDay,
			SysTime.wHour,
			SysTime.wMinute,
			SysTime.wSecond
		};
	}

	return FileTime();
}

void GetFileList(std::vector<std::string>& Ret, const std::string& Path, const std::vector<std::string>& Filters, bool8_t ToLower)
{
	std::string RootPath(Path + "\\*.*");

	::WIN32_FIND_DATAA FindData{};
	::HANDLE Handle = ::FindFirstFileA(RootPath.c_str(), &FindData);

	while (true)
	{
		if (FindData.cFileName[0] != '.')
		{
			if (FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				std::string SubDir = Path + "\\" + FindData.cFileName;
				GetFileList(Ret, SubDir, Filters, ToLower);
			}
			else
			{
				std::string FileName(FindData.cFileName);

				if (Filters.size() == 0u)
				{
					std::string FilePath = Path + "\\" + FileName;
					Ret.emplace_back(ToLower ? StringUtils::Lowercase(FilePath) : std::move(FilePath));
				}
				else
				{
					std::string Ext = File::Extension(FileName, true);
					for (auto It = Filters.cbegin(); It != Filters.cend(); ++It)
					{
						std::string Filter(*It);
						StringUtils::ToLower(Filter);

						if (Ext == Filter)
						{
							std::string FilePath = Path + "\\" + FileName;
							Ret.emplace_back(ToLower ? StringUtils::Lowercase(FilePath) : std::move(FilePath));
							break;
						}
					}
				}
			}
		}

		if (!::FindNextFileA(Handle, &FindData))
		{
			break;
		}
	}
}

void TryToFindFile(const std::string& Path, const std::string& Name, _Inout_ std::string& Ret)
{
	std::string RootPath(Path + "\\*.*");

	::WIN32_FIND_DATAA FindData{};
	::HANDLE Handle = ::FindFirstFileA(RootPath.c_str(), &FindData);

	while (true)
	{
		if (FindData.cFileName[0] != '.')
		{
			if (FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				TryToFindFile(Path + "\\" + FindData.cFileName, Name, Ret);
			}
			else
			{
				if (_stricmp(FindData.cFileName, Name.c_str()) == 0)
				{
					Ret = Path + "\\" + FindData.cFileName;
					break;
				}
			}
		}

		if (!::FindNextFileA(Handle, &FindData))
		{
			break;
		}
	}
}

std::vector<std::string> File::GetFileList(const char8_t* Path, const std::vector<std::string>& Filters, bool8_t ToLower)
{
	std::vector<std::string> Ret;

	if (File::Exists(Path))
	{
		Gear::GetFileList(Ret, std::string(Path), Filters, ToLower);
	}

	return Ret;
}

std::string File::Find(const char8_t* Path, const char8_t* Name)
{
	std::string Ret;
	if (File::Exists(Path))
	{
		TryToFindFile(Path, Name, Ret);
	}

	return Ret;
}

void File::CreateDirectories(const char8_t* Path)
{
	VERIFY_WITH_PLATFORM_MESSAGE(std::filesystem::create_directories(Path));
}

#else
	#error Unknown platform!
#endif
#endif

