#pragma once

#include "Core/Module.h"
#pragma warning(disable:4068)
#include <Submodules/filewatch/FileWatch.hpp>
#pragma warning(default:4068)

class FileWatchService : public IService<FileWatchService>
{
public:
	void OnStartup() override final {}
	virtual void OnShutdown();

	using FileStatusChangedCallback = std::function<void(const std::string&, filewatch::Event)>;

	void RegisterWatcher(const std::string& WatchDirectory, const std::vector<const char* const>& FileExtensions, FileStatusChangedCallback&& Callback);
protected:
private:
	class FileWatcher : public filewatch::FileWatch<std::string>
	{
	private:
		FileStatusChangedCallback m_Callback;
	};

	std::vector<std::unique_ptr<FileWatcher>> m_Watchers;
};


