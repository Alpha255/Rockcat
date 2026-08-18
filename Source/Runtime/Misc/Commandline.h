#pragma once

#include "Core/Definitions.h"

class Commandline
{
public:
	template<class T>
	static void Add(std::string_view Token, const T& Value)
	{
	}

	bool Parse();
private:
	static std::string s_RawCommandline;
};