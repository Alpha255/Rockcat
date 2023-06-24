#include "Runtime/Engine/Asset/SceneAsset.h"
#include "Runtime/Engine/Engine.h"

void SceneGraphAsset::LoadAssimpScenes()
{
	for each (const auto& Path in m_AssimpScenePaths)
	{
		m_AssimpScenes.push_back(
			Engine::Get().GetAssetDatabase().FindAsset<AssimpSceneAsset>(Path));
	}
}
