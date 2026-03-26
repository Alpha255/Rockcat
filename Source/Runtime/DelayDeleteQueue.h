#pragma once

#include "Core/Definitions.h"

class DelayDelete
{
public:
	virtual void Release() = 0;
};

template<class T>
class DelayDeleteQueue
{

};