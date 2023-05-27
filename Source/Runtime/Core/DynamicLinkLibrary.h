#include "Runtime/Core/Definitions.h"

class DynamicLinkLibrary
{
public:
	DynamicLinkLibrary(const char8_t* LibraryName);
	virtual ~DynamicLinkLibrary();

	void* GetProcAddress(const char8_t* FunctionName);
protected:
private:
	void* m_Handle = nullptr;
};