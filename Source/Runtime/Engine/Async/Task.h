#include "Runtime/Core/Definitions.h"

class Task
{
private:
	Task* m_Pre = nullptr;
	Task* m_Succeed = nullptr;
};