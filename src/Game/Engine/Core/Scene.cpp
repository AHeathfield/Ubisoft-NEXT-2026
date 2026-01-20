#include "Scene.h"
#include "ECS.h"
#include "GameManager.h"
#include "Utility.h"

void Scene::Exit()
{
    Coordinator& coordinator = GameManager::Instance().GetCoordinator();
    for (auto it = sceneEntities.begin(); it != sceneEntities.end(); it++)
    {
        // if (coordinator.IsEntityDestroyed(*it))
        //     continue;

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
    if (utility::containsValue(sceneEntities, ent))
    {
        GameManager::Instance().GetCoordinator().DestroyEntity(ent);
        sceneEntities.erase(ent);
    }
}

bool Scene::IsActive()
{
    return mActive;
}

void Scene::SetActive(const bool active)
{
    mActive = active;
}
