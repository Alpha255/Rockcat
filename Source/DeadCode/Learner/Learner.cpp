#include "recursion.hpp"
#include "async.hpp"
#include "sort.hpp"

template <class T>
class Test
{
public:
	Test()
	{
		std::cout << "Construct" << std::endl;
	}

	~Test()
	{
		std::cout << "Destruct" << std::endl;
	}
};

template <class T, size_t Size>
using TestArray = std::array<Test<T>, Size>;

class Pointer
{
};

DECLARE_PTR(Pointer)

int32_t main(int32_t argc, char8_t* argv[])
{
	using TestArray4 = TestArray<uint32_t, 4>;

	auto mem = new char[sizeof(TestArray4::value_type) * 4u]();
	//auto mem = new TestArray4::value_type[4]();
	auto testarray = new (mem) TestArray4();
	delete testarray;
	testarray = nullptr;

	Pointer_Weak Pointer_weak;
	Pointer_Shared Pointer_shared;
	Pointer_Unique Pointer_unique;

	return 0;
}