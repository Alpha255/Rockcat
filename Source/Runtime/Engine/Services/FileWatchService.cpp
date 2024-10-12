#include "Engine/Services/FileWatchService.h"

void FileWatchService::OnShutdown()
{
	m_Watchers.clear();
}

void FileWatchService::RegisterWatcher(const std::string& WatchDirectory, const std::vector<const char* const>& FileExtensions, FileStatusChangedCallback&& Callback)
{
}
