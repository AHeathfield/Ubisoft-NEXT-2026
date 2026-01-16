#include "Scene.h"
#include "GameManager.h"

void Scene::Exit()
{
    for (auto it = sceneEntities.begin(); it != sceneEntities.end(); it++)
    {
        GameManager::Instance().GetCoordinator().DestroyEntity(*it);
    }
    sceneEntities.clear();
}


Entity Scene::CreateEntity()
{
    Entity ent = GameManager::Instance().GetCoordinator().CreateEntity();
    sceneEntities.insert(ent);
    return ent;
}

void Scene::DestroyEntity(Entity ent)
{
    GameManager::Instance().GetCoordinator().DestroyEntity(ent);
    sceneEntities.erase(ent);
}

bool Scene::IsActive()
{
    return mActive;
}

void Scene::SetActive(const bool active)
{
    mActive = active;
}
