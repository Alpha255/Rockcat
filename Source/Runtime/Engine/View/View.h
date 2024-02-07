#pragma once

#include "Core/Definitions.h"

class IView
{
public:
	virtual bool IsStereo() const { return false; }
	virtual bool IsReverseDepth() const { return false; }
};
