#pragma once

#include "Asset/SceneAsset.h"
#include "Core/Tickable.h"

class Scene : public ITickable, public SceneGraph, public SerializableAsset<Scene>
{
public:
	Scene(std::filesystem::path&& SubPath)
		: BaseClass(Paths::ScenePath() / SubPath)
	{
	}

	~Scene();

	static const char* GetExtension() { return ".scene"; }

	void Tick(float ElapsedSeconds) override final;

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_BASE(SceneGraph),
			CEREAL_NVP(m_AssimpScenes)
		);
	}
protected:
	void PostLoad() override;
private:
	std::vector<std::string> m_AssimpScenes;
};

