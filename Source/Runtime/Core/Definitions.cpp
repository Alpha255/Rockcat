#include "Core/Definitions.h"

namespace Utils
{
	size_t PopulationCount(size_t Value)
	{
#if 0
		size_t Count = 0u;
		while (Value != 0u)
		{
			Value &= (Value - 1);
			++Count;
		}
		return Count;
#else
		return ::__popcnt64(Value);
#endif
	}

	size_t GetPowerOfTwo(uint32_t Value)
	{
		if (Value == 0u)
		{
			return 0u;
		}

		if (!IsPowerOfTwo(Value))
		{
			return 0u;
		}

		size_t Count = 0u;

		while (Value >>= 1u)
		{
			++Count;
		}
		return Count;
	}

	std::string GetTimepointString(const std::chrono::system_clock::time_point& Timepoint, const char* Format)
	{
		std::time_t Time = std::chrono::system_clock::to_time_t(Timepoint);
		std::tm LocalTime = *std::localtime(&Time);
		std::ostringstream StringStream;
		StringStream << std::put_time(&LocalTime, Format);
		return StringStream.str();
	}
}
