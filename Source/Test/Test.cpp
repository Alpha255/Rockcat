#include "Runtime/Engine/Application/BaseApplication.h"
#include "Runtime/Engine/Engine.h"
#include "Runtime/Core/Main.h"

class Test : public BaseApplication
{
public:
	using BaseApplication::BaseApplication;
};

REGISTER_APPLICATION(Test, "Test.json")
REGISTER_APPLICATION(Test, "Test1.json")