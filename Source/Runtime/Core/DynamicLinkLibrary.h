#include "Runtime/Core/Definitions.h"

class DynamicLinkLibrary
{
public:
	DynamicLinkLibrary(const char* LibraryName);
	virtual ~DynamicLinkLibrary();

	void* GetProcAddress(const char* FunctionName);
protected:
private:
	void* m_Handle = nullptr;
};