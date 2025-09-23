#include "Runtime/Application/BaseApplication.h"

#if PLATFORM_LINUX

class LinuxApplication : public BaseApplication
{
public:
	using BaseApplication::BaseApplication;
};

using PlatformApplication = LinuxApplication;

#endif