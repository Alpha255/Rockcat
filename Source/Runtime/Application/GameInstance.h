#include "Core/Definitions.h"

class GameInstance
{
public:
	GameInstance(const char* Name)
		: m_Name(Name)
	{
	}

	inline std::string_view GetName() const { return m_Name; }

	virtual void OnBeginPlay() {}

	virtual void Tick(float ElapsedSeconds) {}

	virtual void OnEndPlay() {}

	void OnAttachToViewport(class Viewport& Viewport);
private:
	std::string_view m_Name;
};