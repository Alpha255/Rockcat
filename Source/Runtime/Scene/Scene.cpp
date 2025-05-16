#include "Scene/Scene.h"
#include "Scene/SceneVisitor.h"

void Scene::Tick(float ElapsedSeconds)
{
	(void)ElapsedSeconds;
}

void Scene::PostLoad()
{
}

Scene::~Scene()
{
	RemoveDyingEntities();
	Save(true);
}
