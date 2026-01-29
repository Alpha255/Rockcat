#include "Asset/RenderResource.h"
#include "Core/Singleton.h"

class FGlobalRenderResourceList : public Singleton<FGlobalRenderResourceList>
{
public:
	uint32_t Allocate(RenderResource* Resource)
	{
		std::lock_guard Locker(m_Lock);

		uint32_t Index = NONE_INDEX;
		if (m_FreeIndices.empty())
		{
			Index = static_cast<uint32_t>(m_Resources.size());
			m_Resources.push_back(Resource);
		}
		else
		{
			Index = m_FreeIndices.front();
			m_FreeIndices.pop();
			m_Resources[Index] = Resource;
		}

		return Index;
	}

	void Free(uint32_t Index)
	{
		std::lock_guard Locker(m_Lock);

		m_Resources[Index] = nullptr;
		m_FreeIndices.push(Index);
	}

	template<class Func>
	void ForEach(Func&& Function)
	{
		std::lock_guard Locker(m_Lock);

		std::for_each(m_Resources.begin(), m_Resources.end(), std::forward<Func>(Function));
	}
private:
	std::vector<RenderResource*> m_Resources;
	std::queue<uint32_t> m_FreeIndices;
	std::mutex m_Lock;
};

void RenderResource::CreateGlobalResources()
{
	FGlobalRenderResourceList::Get().ForEach([](RenderResource* Resource) {
		Resource->CreateRHI();
	});
}

void RenderResource::DestroyGlobalResources()
{
	FGlobalRenderResourceList::Get().ForEach([](RenderResource* Resource) {
		Resource->ReleaseRHI();
	});
}

void RenderResource::Initialize()
{
	if (m_Index == NONE_INDEX)
	{
		m_Index = FGlobalRenderResourceList::Get().Allocate(this);
	}
}
