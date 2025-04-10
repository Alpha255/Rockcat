#include "Engine/Scene/Components/Components.h"
#include "Engine/Async/Task.h"

class TestEntity : public Entity2<TransformComponent, StaticMeshComponent>
{
public:
	using Entity2::Entity2;

	void Test()
	{
		if (auto TransformComp = GetComponent<TransformComponent>())
		{
		}
	}
};