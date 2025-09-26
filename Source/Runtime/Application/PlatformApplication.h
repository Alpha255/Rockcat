#pragma once

#include "Core/Definitions.h"

#if PLATFORM_WIN32
	#include "Application/Win32/WindowsApplication.h"
#elif PLATFORM_LINUX
	#include "Application/Linux/LinuxApplication.h"
#else
	#error Unknown platform
#endif
