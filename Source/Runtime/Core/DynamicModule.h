#include "Runtime/Core/Definitions.h"

class DynamicModule
{
public:
	DynamicModule(const char8_t* ModuleName);
	virtual ~DynamicModule();

	void* GetProcAddress(const char8_t* FunctionName);
protected:
private:
	void* m_Handle = nullptr;
};