#include "Runtime/Engine/Engine.h"
#include "Runtime/Engine/Asset/ShaderAsset.h"

int main()
{
	LOG_ERROR("Test");

	auto Test = ShaderCache::Load("ImGui.vert");
	Test->Save();

	MemoryBlock Block("Test.json");
	Block.SizeInBytes = 256u;
	Block.RawData.reset(new byte8_t[256u]);

	std::vector<uint64_t> Data(256 / sizeof(uint64_t));
	for (uint32_t i = 0; i < Data.size(); ++i)
	{
		Data[i] = std::rand();
	}

	memcpy(Block.RawData.get(), Data.data(), 256);

	Block.Save(true);

	auto Test1 = MemoryBlock::Load("Test.json");
	
	return 0;
}